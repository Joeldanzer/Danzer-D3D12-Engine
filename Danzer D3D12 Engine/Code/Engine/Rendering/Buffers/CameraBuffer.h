#pragma once
#include "CBVBuffer.h"
#include "Core/MathDefinitions.h"

class CameraBuffer : public CBVBuffer
{
public:
	CameraBuffer();
	~CameraBuffer();

	void UpdateBuffer(void* cbvData) override;

	// Camera Buffer Data
	struct Data {
		Mat4f  m_transform  = Mat4f::Identity;
		Mat4f  m_projection = Mat4f::Identity;
		Vect4f m_position   = Vect4f::Zero;
		Vect4f m_direction  = Vect4f::Zero;
	};

	Data* FetchData() {
		return &m_cameraBufferData;
	}

private:
	Data m_cameraBufferData;
};

