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
	void ClearBuffer(float red, float green, float blue) noexcept
	{
		const float color[] = { red, green, blue, 1.0f };
		pContext->ClearRenderTargetView(pTarget, color);
		pContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0u); 
	}
	void DrawTestTriangle(float angle, float x, float y);
private:
	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwap = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pTarget = nullptr;
	ID3D11DepthStencilView* pDSV = nullptr;
};