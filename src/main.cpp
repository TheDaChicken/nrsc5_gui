//
// Created by TheDaChicken on 7/13/2025.
//

#include "Application.h"

#ifdef _WIN32
#include <windows.h>

int WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	Application mainWindow;
	mainWindow.Run();
	return 0;
}

#endif

int main()
{
	Application mainWindow;
	mainWindow.Run();
	return 0;
}
