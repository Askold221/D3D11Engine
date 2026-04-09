#include "Graphics.h"
#include <d3dcompiler.h>
#include <iterator>
#include <memory>
#include <cmath>
#include <iostream>
#include <DirectXMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

namespace dx = DirectX;

Graphics::Graphics(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		nullptr,
		&pContext
	);

	ID3D11Resource* pBackBuffer = nullptr;
	pSwap->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer));
	pDevice->CreateRenderTargetView(
		pBackBuffer,
		nullptr,
		&pTarget
	);

	// create depth stencil state
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; 
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	ID3D11DepthStencilState *pDSState = nullptr;
	pDevice->CreateDepthStencilState(&dsDesc, &pDSState);

	// bind depth state
	pContext->OMSetDepthStencilState(pDSState, 1u);

	// create depth stencil texture
	ID3D11Texture2D* pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = 800u;
	descDepth.Height = 600u;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);

	// create view of depth stencil texture
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;
	pDevice->CreateDepthStencilView(pDepthStencil, &descDSV, &pDSV);

	// bind depth stencil view to OM
	pContext->OMSetRenderTargets(1u, std::addressof(pTarget), pDSV);

 	pBackBuffer->Release();
}

Graphics::~Graphics()
{
	safe_release(pTarget);
	safe_release(pDevice);
	safe_release(pContext);
	safe_release(pSwap);
}

void Graphics::DrawTestTriangle(float angle, float x, float y)
{
	HRESULT hr;
	struct Vertex
	{
		struct
		{
			float x;
			float y;
			float z;
		} pos;
	};

	const Vertex vertices[] =
	{
		{-1.0f, -1.0f, -1.0f}, 
		{1.0f, -1.0f, -1.0f},
		{-1.0f, 1.0f, -1.0f},
		{1.0f, 1.0f, -1.0f},
		{-1.0f, -1.0f, 1.0f},
		{1.0f, -1.0f, 1.0f},
		{-1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f}
	};
	ID3DBlob* pBlob = nullptr; 

	ID3D11Buffer* pVertexBuffer = nullptr;
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeof(vertices);
	bd.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices;
	pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer);
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;

	pContext->IASetVertexBuffers(0u, 1u, std::addressof(pVertexBuffer), &stride, &offset);

	//create index buffer
	const unsigned short indices[] =
	{
		0,2,1,2,3,1,
		1,3,5,3,7,5,
		2,6,3,3,6,7,
		4,5,7,4,7,6,
		0,4,2,2,4,6,
		0,1,4,1,5,4
	};
	ID3D11Buffer* pIndexBuffer;
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = sizeof(indices);
	ibd.StructureByteStride = sizeof(unsigned short);
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices;
	pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer);

	//bind index buffer
	pContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0u);

	// create constant buffer
	struct ConstantBuffer {
		dx::XMMATRIX transform;

	};
	
	const ConstantBuffer cb =
	{
		{
			dx::XMMatrixTranspose(
				dx::XMMatrixRotationX(angle) * 
				dx::XMMatrixTranslation(x, y, 4.0f) *
				dx::XMMatrixPerspectiveFovLH( 1.0f, 3.0f / 4.0f, 0.5f, 10.0f) 
			)
		}
	};
	

	ID3D11Buffer* pConstantBuffer;
	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER; 
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(cb);
	cbd.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &cb;
	pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer);

	// bind constant buffer to vertex shader
	pContext->VSSetConstantBuffers(0u, 1u, &pConstantBuffer);
	
	struct ConstantBuffer2
	{
		struct
		{
			float r;
			float g; 
			float b;
			float a;
		} face_colors[6];
	};
	const ConstantBuffer2 cb2 =
	{
		{
		{1.0f, 0.0f, 1.0f},
		{1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f},
		{1.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 1.0f}
		}
	}; 
	ID3D11Buffer* pConstantBuffer2 = nullptr;
	D3D11_BUFFER_DESC cbd2;
	cbd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd2.Usage = D3D11_USAGE_DEFAULT;
	cbd2.CPUAccessFlags = 0u;
	cbd2.MiscFlags = 0u;
	cbd2.ByteWidth = sizeof(cb2);
	cbd2.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd2 = {};
	csd2.pSysMem = &cb2;
	pDevice->CreateBuffer(&cbd2, &csd2, &pConstantBuffer2);

	//bind constant buffer to pixel shader
	pContext->PSSetConstantBuffers(0u, 1u, &pConstantBuffer2);

	//Create Pixel Shader
	ID3D11PixelShader* pPixelShader = nullptr;
	D3DReadFileToBlob(L"PixelShader.cso", &pBlob);
	pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(),
		nullptr, &pPixelShader);

	//bind pixel shader
	pContext->PSSetShader(pPixelShader, nullptr, 0u);

	//Create Vertex Shader
	ID3D11VertexShader* pVertexShader = nullptr;
	
	D3DReadFileToBlob(L"VertexShader.cso", &pBlob);
	pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(),
		nullptr, &pVertexShader);

	//bind vertex shader
	pContext->VSSetShader(pVertexShader, nullptr, 0u);

	// input layout
	ID3D11InputLayout* pInputLayout = nullptr; 
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},	};
		pDevice->CreateInputLayout(
		ied, (UINT)std::size(ied),
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		&pInputLayout
	);

	// bind vertex layout
	pContext->IASetInputLayout(pInputLayout);
	
	// Primitive topology
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = 800;
	vp.Height = 600;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1u, &vp);

	pContext->DrawIndexed((UINT)std::size(indices), 0u, 0u);
}
void Graphics::EndFrame()
{
	pSwap->Present(1u, 0u);
}