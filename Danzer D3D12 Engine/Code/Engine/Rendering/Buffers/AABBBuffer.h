#pragma once
#include "VertexBuffer.h"
#include "Core/MathDefinitions.h"

class AABBBuffer : public VertexBuffer
{
public:
	struct AABBInstance {
		Vect3f m_min;
		Vect3f m_max;
	};

	AABBBuffer(){}
	~AABBBuffer(){}

	void Init(ID3D12Device* device) override;
	void UpdateBuffer(UINT8* c, UINT size, UINT frameIndex) override;
};

