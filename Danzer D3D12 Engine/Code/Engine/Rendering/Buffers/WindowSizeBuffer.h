#pragma once
#include "CBVBuffer.h"
#include "../../Core/MathDefinitions.h"

class WindowBuffer : public CBVBuffer
{
public:
	WindowBuffer();
	~WindowBuffer();

	void Init(ID3D12Device* device) override;
	void UpdateBuffer(/*ID3D12GraphicsCommandList* cmdList,*/ UINT frameIndex, void* cbvData) override;

	struct Data {
		Vect2f m_windowSize;
	};

private :
	Data m_windowData;
};

