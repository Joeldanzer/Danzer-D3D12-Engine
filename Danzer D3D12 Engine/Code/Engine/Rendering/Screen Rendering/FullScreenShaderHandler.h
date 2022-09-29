#pragma once
#include "FullscreenShader.h"

class DirectX12Framework;

class FullScreenShaderHandler
{
public:
	FullScreenShaderHandler(DirectX12Framework& framework);
	~FullScreenShaderHandler();

	FullScreenShader& CreateFullScreenShader(std::wstring pixelShader);
	

private:
	DirectX12Framework& m_framework;
};

