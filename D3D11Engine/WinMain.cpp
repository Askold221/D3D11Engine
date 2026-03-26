#ifndef UNICODE
#define UNICODE
#endif 

#include "App.h"

int WINAPI wWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PWSTR lpCmdLine,
	int nCmdShow)
{
	return App{}.Go();
}
