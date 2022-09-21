#pragma once
#include "CBVBuffer.h"


class SpriteSheetBuffer : public CBVBuffer
{
public:
	SpriteSheetBuffer();
	~SpriteSheetBuffer();

	void Init(ID3D12Device* device) override;
	void UpdateBuffer(/*ID3D12GraphicsCommandList* cmdList,*/ UINT frameIndex, void* cbvData) override;

	struct Data {
		//Vect2f m_spriteSheetSize;
	};

private:
	Data m_windowData;
};

