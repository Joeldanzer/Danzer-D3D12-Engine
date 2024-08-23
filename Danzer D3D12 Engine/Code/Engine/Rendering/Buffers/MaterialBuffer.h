#pragma once
#include "CBVBuffer.h"
#include "Core/MathDefinitions.h"

class MaterialBuffer : public CBVBuffer
{
public:
	// Camera Buffer Data
	struct Data {	
		float m_metallic  = 0.0f;
		float m_roughness = 0.0f;
		float m_emissive  = 0.0f;

		float m_color[4] = { 1.f, 1.f, 1.f, 1.f };
	};

	Data* FetchData() {
		return &m_materialBufferData;
	}

private:
	Data m_materialBufferData;
};
