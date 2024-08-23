#pragma once
#include "CBVBuffer.h"
#include "Core/MathDefinitions.h"

class SSAOBuffer : public CBVBuffer
{
public:
	struct Data {
		Vect4f m_samples[32];
	};

	Data* FetchData() {
		return &m_data;
	}

private:
	Data m_data;
};

