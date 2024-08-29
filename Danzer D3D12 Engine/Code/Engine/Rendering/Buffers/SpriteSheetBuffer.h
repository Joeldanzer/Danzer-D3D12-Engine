#pragma once
#include "CBVBuffer.h"


class SpriteSheetBuffer : public CBVBuffer
{
public:
	struct Data {
		//Vect2f m_spriteSheetSize;
	};

	Data* FetchData() {
		return &m_spriteSheetData;
	}

private:
	Data m_spriteSheetData;
};

