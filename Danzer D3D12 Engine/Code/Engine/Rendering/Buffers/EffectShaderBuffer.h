#pragma once
#include "CBVBuffer.h"
#include "Core/MathDefinitions.h"

class EffectShaderBuffer : public CBVBuffer
{
public:
	void SetDataSize(UINT size){
		m_sizeOfData = size;
	}

	struct Data {
		float m_randomData[64]; // 256 bytes alligned
	};

	Data* FetchData() {
		return &m_data;
	}
private:
	Data m_data;
	UINT m_sizeOfData;
};

