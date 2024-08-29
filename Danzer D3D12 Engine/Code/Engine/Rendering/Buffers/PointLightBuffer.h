#pragma once
#include "CBVBuffer.h"
#include "Core/MathDefinitions.h"

class PointLightBuffer : public CBVBuffer
{
public:
	struct Data {
		Vect4f m_color = Vect4f::Zero;
		float  m_range = 0.0f;
		Vect3f m_position = Vect3f::Zero;

		Vect4f trash[14];
	};

	Data* FetchData() {
		return &m_lightBufferData;
	}

private:
	Data m_lightBufferData;
};


