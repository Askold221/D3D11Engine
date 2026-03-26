#pragma once
#include "motherfucker_headers.h"
#include "Keyboard.h"
#include <string>
#include "Graphics.h"
#include <memory>

class Window {
private:
	class WindowClass
	{
	public:
		static const WCHAR* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const WCHAR* wndClassName = L"My Direct3D Engine Window";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};
public:
	Window(int width, int height, const WCHAR* name) noexcept;
	~Window();
	Window(const Window&) = delete;
	Window& operator = (const Window&) = delete;
	void SetTitle(const std::string& title);
	static std::optional<int> ProcessMessages() noexcept;
	Graphics& Gfx() { return *pGfx; };

	int GetWindowWidth() const { return width; }
	int GetWindowHeight() const { return height; }

	inline void SetWindowWidth(int w) { width = w; }
	inline void SetWindowHeight(int h) { height = h; }

private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	std::unique_ptr<Keyboard> kbd;
private:
	int width;
	int height;
	HWND hWnd;
	std::unique_ptr<Graphics> pGfx;
};