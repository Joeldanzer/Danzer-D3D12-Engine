#pragma once
#include "CBVBuffer.h"
#include "Core/MathDefinitions.h"

class EffectShaderBuffer : public CBVBuffer
{
public:
	EffectShaderBuffer();
	~EffectShaderBuffer();

	void UpdateBuffer(void* cbvData, unsigned int frame) override;
	void SetDataSize(UINT size){
		m_sizeOfData = size;
	}

	struct Data {
		Vect4f m_randomData[16]; // 256 bytes alligned
	};

	Data* FetchData() {
		return &m_data;
	}
private:
	Data m_data;
	UINT m_sizeOfData;
};

