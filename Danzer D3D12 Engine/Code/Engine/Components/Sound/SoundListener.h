#pragma once

static const char s_invalidListener = 0xff;

struct SoundListener {
	SoundListener() : m_id(s_invalidListener) {};
	~SoundListener() {};
	//explicit SoundListener(SoundListener&) = default;

private:
	friend class SoundEngine;
	SoundListener(unsigned int listenerID);

	unsigned char m_id = 0xff;
};