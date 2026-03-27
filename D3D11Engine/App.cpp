#include "App.h"

App::App() : wnd(std::make_unique<Window>(800, 600, L"D3D11Engine"))
{
}

int App::Go()
{
	while (true)
	{
		// process all messages pending, but to not block for new messages
		if (const auto ecode = Window::ProcessMessages())
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}

		DoFrame();
	}
}

void App::DoFrame()
{
	wnd->Gfx().ClearBuffer(0, 0, 256);
	wnd->Gfx().EndFrame();
}
