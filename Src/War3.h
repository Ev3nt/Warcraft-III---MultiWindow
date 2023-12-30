#pragma once

#include "function.h"

class War3 {
public:
	static War3& getInstance();

	~War3();

	void launch(bool clipMouse);

private:
	War3();

	static HWND CALLBACK CreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
	static BOOL CALLBACK SetWindowTextA(HWND hWnd, LPCSTR lpString);

	static HCURSOR WINAPI LoadCursorA(HINSTANCE hInstance, LPCSTR lpCursorName);
	static HANDLE WINAPI LoadImageA(HINSTANCE hInst, LPCSTR name, UINT type, int cx, int cy, UINT fuLoad);

	static HANDLE WINAPI CreateEventA(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCSTR lpName);

	static BOOL WINAPI bind(SOCKET s, sockaddr_in* name, int namelen);
	static BOOL WINAPI sendto(SOCKET s, LPCSTR buf, int len, int flags, sockaddr_in* to, int tolen);

	static DWORD WINAPI StormGetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh);

	static LRESULT WINAPI CallWndProc(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI CallKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

	static void bindCursorAsync();

	static WORD m_tcp_port;

	MODULE m_gameBase;
	static HWND m_gameWindow;
	static HHOOK m_windowHook, m_keyboardHook;
	static bool m_clipMouse;
};