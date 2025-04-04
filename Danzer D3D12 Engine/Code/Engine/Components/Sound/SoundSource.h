#pragma once

namespace FMOD {
	class Channel;
}

struct SoundSource {
	SoundSource(){}

private:
	Vect3f m_lastPosition;
	std::vector<FMOD::Channel*> m_channels;
	friend class SoundEngine;
};


