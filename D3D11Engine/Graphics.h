#pragma once
#include "motherfucker_headers.h"
#include <d3d11.h>

#define safe_release(a)	{	if(a){(a)->Release(); (a)=nullptr;}	}

class Graphics {
public:
	Graphics(HWND hWnd);
	Graphics() = default;
	Graphics(const Graphics&) = delete;
	Graphics& operator = (const Graphics&) = delete;
	~Graphics();
	void EndFrame();
private:
	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwap = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
};