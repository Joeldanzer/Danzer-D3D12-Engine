#pragma once
#include "FullscreenShader.h"

class D3D12Framework;

class FullScreenShaderHandler
{
public:
	FullScreenShaderHandler(D3D12Framework& framework);
	~FullScreenShaderHandler();

	FullScreenShader& CreateFullScreenShader(std::wstring pixelShader);
	

private:
	D3D12Framework& m_framework;
};

