#pragma once
#include "CBVBuffer.h"
#include "Core/MathDefinitions.h"

class LightBuffer : public CBVBuffer
{
public:
	LightBuffer();
	~LightBuffer();

	void UpdateBuffer(void* cbvData, unsigned int frame) override;

	// Camera Buffer Data
	struct Data {
		Mat4f  m_projection;
		Mat4f  m_transform;
		Vect4f m_lightColor;
		Vect4f m_ambientColor;
		Vect4f m_lightDirection;
	};

	Data* FetchData() {
		return &m_lightBufferData;
	}

private:
	Data m_lightBufferData;
};



