#pragma once
#include "Components/ComponentRegister.h"

static const UINT s_invalidListener = 0xff;

struct SoundListener : public BaseComponent {
	COMP_FUNC(SoundListener)

	SoundListener() : m_id(s_invalidListener) {};
	~SoundListener() {};
	//explicit SoundListener(SoundListener&) = default;

private:
	friend class SoundEngine;
	SoundListener(UINT8 listenerID);

	Vect3f m_lastPosition;
	UINT8  m_id = 0xff;
};
REGISTER_COMPONENT(SoundListener)