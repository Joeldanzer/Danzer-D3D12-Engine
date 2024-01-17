#pragma once
#include "CBVBuffer.h"

class EffectShaderBuffer : CBVBuffer
{
	EffectShaderBuffer();
	~EffectShaderBuffer();

	void UpdateBuffer(void* cbvData, unsigned int frame) override;
	void SetDataSize(UINT size){
		m_sizeOfData = size;
	}
private:
	UINT m_sizeOfData;
};

