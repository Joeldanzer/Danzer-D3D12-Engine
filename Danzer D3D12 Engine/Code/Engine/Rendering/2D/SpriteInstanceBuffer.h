#pragma once
#include "../Buffers/VertexBuffer.h"

class SpriteInstanceBuffer : public VertexBuffer
{
public:
	SpriteInstanceBuffer();
	~SpriteInstanceBuffer();

	void Init(ID3D12Device* device) override;
	void UpdateBuffer(UINT8* instances, UINT size, UINT frameIndex) override;

private:

};

