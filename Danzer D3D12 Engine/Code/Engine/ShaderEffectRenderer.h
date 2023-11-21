#pragma once

class D3D12Framework;
struct ID3D12GraphicsCommandList;

class ShaderEffectRenderer
{
public:
	ShaderEffectRenderer(D3D12Framework& framework);

private:
	D3D12Framework& m_framework;
	ID3D12GraphicsCommandList* m_cmdList;
};

