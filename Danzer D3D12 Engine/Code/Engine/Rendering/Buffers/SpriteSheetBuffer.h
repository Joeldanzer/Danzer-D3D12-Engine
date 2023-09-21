#pragma once
#include "CBVBuffer.h"


class SpriteSheetBuffer : public CBVBuffer
{
public:
	SpriteSheetBuffer();
	~SpriteSheetBuffer();

	//void Init(ID3D12Device* device, DescriptorHeapWrapper* cbvWrapper) override;
	void UpdateBuffer(void* cbvData) override;

	struct Data {
		//Vect2f m_spriteSheetSize;
	};

	Data* FetchData() {
		return &m_spriteSheetData;
	}

private:
	Data m_spriteSheetData;
};

