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

	static HCURSOR CALLBACK LoadCursorA(HINSTANCE hInstance, LPCSTR lpCursorName);
	static HANDLE CALLBACK LoadImageA(HINSTANCE hInst, LPCSTR name, UINT type, int cx, int cy, UINT fuLoad);

	static HANDLE CALLBACK CreateEventA(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCSTR lpName);

	static BOOL CALLBACK bind(SOCKET s, sockaddr_in* name, int namelen);
	static BOOL CALLBACK sendto(SOCKET s, LPCSTR buf, int len, int flags, sockaddr_in* to, int tolen);

	static DWORD CALLBACK StormGetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh);

	static HRESULT CALLBACK WndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK CallKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

	static void bindCursorAsync();

	static WORD m_tcp_port;

	MODULE m_gameBase;
	static HWND m_gameWindow;
	static WNDPROC m_wndProc;
	static bool m_clipMouse;
};