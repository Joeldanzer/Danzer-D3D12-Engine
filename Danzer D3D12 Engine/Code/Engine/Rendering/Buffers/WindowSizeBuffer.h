#pragma once
#include "CBVBuffer.h"
#include "../../Core/MathDefinitions.h"

class WindowBuffer : public CBVBuffer
{
public:
	struct Data {
		Vect2f m_windowSize;
	};

	Data* FetchData() {
		return &m_windowData;
	}

private :
	Data m_windowData;
};

