#pragma once
#include <vector>
#include "Core/D3D12Header.h"
#include "DirectX/include/directx/d3d12.h"
#include "Core/MathDefinitions.h"
#include "../Buffers/EffectShaderBuffer.h"
#include "Core/D3D12Framework.h"

class ModelEffectData {
public:
	ModelEffectData()  = delete;
	~ModelEffectData() = default;
	explicit ModelEffectData(const ModelEffectData&) = default;
	explicit ModelEffectData(const UINT model, std::vector<UINT> textures, bool hasBuffer, D3D12Framework& framework) :
		m_model(model), m_textures(textures),
		m_hasBuffer(hasBuffer),
		m_bufferData(),
		m_sizeOfData(0),
		m_pso(0),
		m_rs(0)
	{
		if (hasBuffer)
			m_buffer.Init(framework.GetDevice(), &framework.CbvSrvHeap(), m_buffer.FetchData(), sizeof(EffectShaderBuffer::Data));
	}

	const UINT ModelID() {
		return m_model;
	}

	const bool HasBuffer() {
		return m_hasBuffer;
	}

	EffectShaderBuffer& GetBuffer() {
		return m_buffer;
	}
	
	void UpdateData(UINT frameIndex) {
		m_buffer.UpdateBuffer(&m_bufferData, frameIndex);
	}

	const std::vector<UINT>& GetTextures() {
		return m_textures;
	}

	std::vector<Mat4f>& GetTransforms() {
		return m_transforms;
	}

	const UINT GetPSO() {
		return m_pso;
	}
	const UINT GetRootsSignature() {
		return m_rs;
	}
private:
	friend class ModelEffectHandler; //

	UINT m_model;
	std::vector<UINT> m_textures;
	std::vector<Mat4f> m_transforms;

	UINT m_rs;
	UINT m_pso;

	EffectShaderBuffer::Data m_bufferData;
	UINT m_sizeOfData;

	bool m_hasBuffer;
	EffectShaderBuffer m_buffer;
};