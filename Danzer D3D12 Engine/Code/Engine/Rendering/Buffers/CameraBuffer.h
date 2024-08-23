#pragma once
#include "CBVBuffer.h"
#include "Core/MathDefinitions.h"

class CameraBuffer : public CBVBuffer
{
public:
	//CameraBuffer();
	//~CameraBuffer();

	//void UpdateBuffer(void* cbvData, unsigned int frame) override;

	// Camera Buffer Data
	struct Data {
		Mat4f  m_projection = Mat4f::Identity;
		Mat4f  m_transform  = Mat4f::Identity;
		Vect4f m_position   = Vect4f::Zero;
		Vect4f m_direction  = Vect4f::Zero;
		float  m_time = 0.0f;
		float  m_width = 0.0f;
		float  m_height = 0.0f;
	};

	Data* FetchData() {
		return &m_cameraBufferData;
	}

private:
	Data m_cameraBufferData;
};

