#pragma once
#include "../Buffers/VertexBuffer.h"

class TransformBuffer : public VertexBuffer
{
public:
	TransformBuffer();

	void Init(ID3D12Device* device) override;
	void UpdateBuffer(UINT8* transforms, UINT size, UINT frameIndex) override;
};


