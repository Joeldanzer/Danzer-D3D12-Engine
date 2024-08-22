#pragma once

namespace FMOD {
	class System;
}

class SoundHandler;

class SoundEngine
{
public:
	static constexpr UINT16 s_maxSoundChannels  = 512;
	static constexpr UINT8  s_maxNum3DListeners = 4;

public:
	SoundEngine();
	~SoundEngine();

	void UpdateSound(entt::registry& registry, const float dt);

private:
	FMOD::System* m_soundSystem = nullptr;
};

