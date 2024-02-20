#pragma once
#include "CBVBuffer.h"
#include "Core/MathDefinitions.h"

class SSAOBuffer : public CBVBuffer
{
public:
	SSAOBuffer() : m_data() {}
	~SSAOBuffer(){}

	void UpdateBuffer(void* cbvData, unsigned int frame) override;

	// Camera Buffer Data
	struct Data {
		Vect3f m_samples[32];
		Vect2f m_noiseScale;

		//float m_trash[62];
	};

	Data* FetchData() {
		return &m_data;
	}

private:
	Data m_data;
};

