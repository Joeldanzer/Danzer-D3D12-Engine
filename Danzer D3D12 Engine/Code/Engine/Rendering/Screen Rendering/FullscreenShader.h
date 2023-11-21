#pragma once
#include <string>

class D3D12Framework;
struct ID3D12GraphicCommandList;

class FullScreenShader
{
public:
	FullScreenShader(D3D12Framework& framework);
	~FullScreenShader();

	
private:
	D3D12Framework& m_framework;
	ID3D12GraphicCommandList* m_cmdList;
	
};

