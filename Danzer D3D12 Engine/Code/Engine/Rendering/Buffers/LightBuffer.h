#pragma once
#include "CBVBuffer.h"
#include "Core/MathDefinitions.h"

class LightBuffer : public CBVBuffer
{
public:
	LightBuffer();
	~LightBuffer();

	void Init(ID3D12Device* device) override;
	void UpdateBuffer(UINT frameIndex, void* cbvData) override;

	// Camera Buffer Data
	struct Data {
		Vect4f m_lightColor;
		Vect4f m_ambientColor;
		Vect4f m_lightDirection;
	};

private:
	Data m_lightBufferData;
};



