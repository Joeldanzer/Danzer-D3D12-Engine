#pragma once

namespace FMOD {
	class Channel;
}

struct SoundSource {
	SoundSource(){}

private:
	std::vector<FMOD::Channel*> m_channels;
	friend class SoundEngine;
};


