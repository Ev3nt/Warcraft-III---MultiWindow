#include <Windows.h>
#include <stdexcept>

#include "Application.h"

BOOL APIENTRY WinMain(HINSTANCE instance, HINSTANCE instancePrevious, LPSTR cmdLine, int cmdShow) {
	try {
		Application& application = Application::getInstance();
		application.run(cmdLine);
	}
	catch (const std::exception& exception) {
		MessageBox(nullptr, exception.what(), "Ошибка", MB_ICONHAND);
	}

	return FALSE;
}