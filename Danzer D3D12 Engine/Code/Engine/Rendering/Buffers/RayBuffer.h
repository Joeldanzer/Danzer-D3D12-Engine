#pragma once
#include "VertexBuffer.h"
#include "Core/MathDefinitions.h"

class RayBuffer : public VertexBuffer
{
public:
	struct RayInstance {
		Vect3f m_start;
		Vect3f m_end;
	};

	RayBuffer(){}
	~RayBuffer(){}

	void Init(ID3D12Device* device) override;
	void UpdateBuffer(UINT8* c, UINT size, UINT frameIndex) override;
};

