#pragma once
#include "CBVBuffer.h"
#include "Core/MathDefinitions.h"

class PointLightBuffer : public CBVBuffer
{
public:
	void UpdateBuffer(void* cbvData, unsigned int frame) override;

	// Camera Buffer Data
	struct Data {
		Vect4f m_color;
		float  m_range;
		Vect3f m_position;
	};

	Data* FetchData() {
		return &m_lightBufferData;
	}

private:
	Data m_lightBufferData;
};


