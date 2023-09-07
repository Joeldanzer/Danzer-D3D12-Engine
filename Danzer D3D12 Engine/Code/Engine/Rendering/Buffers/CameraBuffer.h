#pragma once
#include "CBVBuffer.h"
#include "Core/MathDefinitions.h"

class CameraBuffer : public CBVBuffer
{
public:
	CameraBuffer();
	~CameraBuffer();

	void Init(ID3D12Device* device, DescriptorHeapWrapper* cbvWrapper) override;
	void UpdateBuffer(UINT frameIndex, void* cbvData) override;

	// Camera Buffer Data
	struct Data {
		Mat4f m_transform;
		Mat4f m_projection;
		Vect4f m_position;
		Vect4f m_direction;
	};

private:
	Data m_cameraBufferData;
};

