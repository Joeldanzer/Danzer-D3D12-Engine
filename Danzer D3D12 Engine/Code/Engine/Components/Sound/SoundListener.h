#pragma once

static const UINT s_invalidListener = 0xff;

struct SoundListener {
	SoundListener() : m_id(s_invalidListener) {};
	~SoundListener() {};
	//explicit SoundListener(SoundListener&) = default;

private:
	friend class SoundEngine;
	SoundListener(UINT8 listenerID);

	UINT8 m_id = 0xff;
};