#include "War3.h"

#include <Windows.h>
#include <stdexcept>
#include "HookManager.h"
#include "Storm.h"
#include <thread>

#include "resource.h"

#define WAR3_NAME "Warcraft III - MultiWindow"

War3& War3::getInstance() {
	static War3 war3;

	return war3;
}

War3::War3(): m_gameBase(LoadLibrary("game.dll")) {
	if (!m_gameBase) {
		throw std::runtime_error("Не удаётся найти game.dll.");
	}

	HookManager::Attach(::CreateWindowExA, War3::CreateWindowExA);
	HookManager::Attach(::SetWindowTextA, War3::SetWindowTextA);

	HookManager::Attach(::LoadCursorA, War3::LoadCursorA);
	HookManager::Attach(::LoadImageA, War3::LoadImageA);

	HookManager::Attach(::CreateEventA, War3::CreateEventA);

	HookManager::Attach(::bind, War3::bind);
	HookManager::Attach(::sendto, War3::sendto);

	HookManager::Attach(::StormGetFileSize, War3::StormGetFileSize);
}

War3::~War3() {
	if (m_gameBase) {
		FreeLibrary(m_gameBase.module);
	}

	UnhookWindowsHookEx(m_windowHook);
	UnhookWindowsHookEx(m_keyboardHook);
	ClipCursor(nullptr);
}

bool War3::m_clipMouse = false;
void War3::launch(bool clipMouse) {
	m_clipMouse = clipMouse;

	StormOpenArchive("", 0, 0, nullptr);

	sfunction<BOOL(HMODULE)> GameMain(GetProcAddress(m_gameBase.module, "GameMain"));
	GameMain(m_gameBase.module);
}

HWND War3::m_gameWindow = nullptr;
HHOOK War3::m_windowHook = nullptr;
HHOOK War3::m_keyboardHook = nullptr;
HWND CALLBACK War3::CreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
	m_gameWindow = HookManager::Invoke(War3::CreateWindowExA, dwExStyle, lpClassName, WAR3_NAME, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	
	m_windowHook = SetWindowsHook(WH_CALLWNDPROC, CallWndProc);
	m_keyboardHook = SetWindowsHook(WH_KEYBOARD, CallKeyboardProc);

	return m_gameWindow;
}

BOOL CALLBACK War3::SetWindowTextA(HWND hWnd, LPCSTR lpString) {
	return HookManager::Invoke(War3::SetWindowTextA, hWnd, WAR3_NAME);
}

HCURSOR WINAPI War3::LoadCursorA(HINSTANCE hInstance, LPCSTR lpCursorName) {
	if (!_strcmpi(lpCursorName, "blizzardcursor.cur")) {
		return HookManager::Invoke(War3::LoadCursorA, hInstance, (LPCSTR)MAKEINTRESOURCE(MULTIWINDOW_CURSOR));
	}

	return HookManager::Invoke(War3::LoadCursorA, hInstance, lpCursorName);
}

HANDLE WINAPI War3::LoadImageA(HINSTANCE hInst, LPCSTR name, UINT type, int cx, int cy, UINT fuLoad) {

	return HookManager::Invoke(War3::LoadImageA, hInst, (LPCSTR)MAKEINTRESOURCE(MULTIWINDOW_ICON), type, cx, cy, (UINT)LR_DEFAULTSIZE);
}

HANDLE WINAPI War3::CreateEventA(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCSTR lpName) {
	HANDLE result = HookManager::Invoke(War3::CreateEventA, lpEventAttributes, bManualReset, bInitialState, lpName);

	if (lpName && !strcmp(lpName, "Warcraft III Game Application")) {
		SetLastError(NULL);
	}

	return result;
}

WORD War3::m_tcp_port;
BOOL WINAPI War3::bind(SOCKET s, sockaddr_in* name, int namelen) {
	int optVal;
	int optLen = sizeof(int);

	if (getsockopt(s, SOL_SOCKET, SO_TYPE, (LPSTR)&optVal, &optLen) != SOCKET_ERROR) {
		switch (optVal)
		{
		case SOCK_DGRAM:
			if (name->sin_port == 0xE017) {
				optVal = TRUE;
				setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (LPCSTR)&optVal, sizeof(optVal));

				return HookManager::Invoke(War3::bind, s, name, namelen);
			}

			break;

		case SOCK_STREAM:
			optVal = FALSE;
			setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&optVal, sizeof(optVal));

			WORD port = ntohs(name->sin_port);

			while (HookManager::Invoke(War3::bind, s, name, namelen) == SOCKET_ERROR) {
				name->sin_port = htons(++port);
			}

			m_tcp_port = port;

			return NULL;
		}
	}

	return HookManager::Invoke(War3::bind, s, name, namelen);
}

BOOL WINAPI War3::sendto(SOCKET s, LPCSTR buf, int len, int flags, sockaddr_in* to, int tolen) {
	struct War3_Packet {
		BYTE signature;
		BYTE command;
		WORD size;
	};

	War3_Packet* data = (War3_Packet*)buf;

	if (len >= sizeof(War3_Packet) && data->signature == 0xF7 && data->command == 0x30) {
		*(WORD*)(&buf[data->size - 2]) = m_tcp_port;
		sockaddr_in addr = *to;
		addr.sin_addr.S_un.S_addr = inet_addr("255.255.255.255");

		return HookManager::Invoke(War3::sendto, s, buf, len, flags, &addr, (int)sizeof(addr));
	}

	return HookManager::Invoke(War3::sendto, s, buf, len, flags, to, tolen);
}

DWORD WINAPI War3::StormGetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh) {
	DWORD result = HookManager::Invoke(War3::StormGetFileSize, hFile, lpFileSizeHigh);

	if (result) {
		std::string fileName(MAX_PATH, NULL);
		StormGetFileName(hFile, fileName.data(), MAX_PATH);

		if (!_strcmpi(&fileName[fileName.size() - 4], ".w3x") || !_strcmpi(&fileName[fileName.size() - 4], ".w3m")) {
			return 1;
		}
	}

	return result;
}

void War3::bindCursorAsync() {
	if (!m_clipMouse) {
		return;
	}

	std::thread([&]() {
		RECT rect;
		GetWindowRect(m_gameWindow, &rect);
		ClipCursor(&rect);
		}).detach();
}

LRESULT WINAPI War3::CallWndProc(int nCode, WPARAM wParam, LPARAM lParam) {
	static bool hookOn = false, sysCmdOn = false;

	if (nCode >= HC_ACTION) {
		CWPSTRUCT* msg = (CWPSTRUCT*)lParam;

		switch (msg->message)
		{
		case WM_SYSCOMMAND:
			if (wParam == SC_MAXIMIZE) {
				hookOn = false;
				sysCmdOn = false;
			}
			else {
				sysCmdOn = true;
			}

			break;

		case WM_CAPTURECHANGED:
			if (!sysCmdOn && !hookOn) {
				hookOn = true;
				bindCursorAsync();
			}
			else {
				sysCmdOn = false;
			}

			break;

		case WM_SIZE:
			hookOn = true;
			sysCmdOn = false;
			bindCursorAsync();

			break;

		case WM_EXITSIZEMOVE:
			hookOn = true;
			sysCmdOn = false;
			bindCursorAsync();

			break;

		case WM_SETFOCUS:
			bindCursorAsync();

			break;

		case WM_KILLFOCUS:
			hookOn = false;
			ClipCursor(nullptr);

			break;
		default:
			break;
		}
	}

	return CallNextHookEx(m_windowHook, nCode, wParam, lParam);
}

LRESULT WINAPI War3::CallKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	static bool clicked = false;

	if (nCode >= HC_ACTION) {
		WORD keyFlags = HIWORD(lParam);

		if (wParam == VK_F7 && !(keyFlags & KF_UP)) {
			clicked = !clicked;

			if (clicked) {
				m_clipMouse = !m_clipMouse;

				if (m_clipMouse) {
					bindCursorAsync();
				}
				else {
					ClipCursor(nullptr);
				}
			}
		}
	}

	return CallNextHookEx(m_keyboardHook, nCode, wParam, lParam);
}