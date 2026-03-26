#pragma once
#include "Window.h"
#include <memory>

class App
{
public:
	App();
	int Go();
private:
	void DoFrame();
private:
	std::unique_ptr<Window> wnd;
};