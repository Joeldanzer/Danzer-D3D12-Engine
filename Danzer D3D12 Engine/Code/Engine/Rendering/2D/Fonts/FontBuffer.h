#pragma once
#include "../../Buffers/VertexBuffer.h"

class FontBuffer : public VertexBuffer
{
public:
	FontBuffer();
	
	void Init(ID3D12Device* device) override;
	void UpdateBuffer(UINT8* bufferData, UINT size, UINT frameIndex) override;

private:

};

