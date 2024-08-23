#pragma once

namespace FMOD {
	class System;
	class Sound;
}

struct SoundListener;
struct FMOD_CREATESOUNDEXINFO;
class SoundHandler;

class SoundEngine
{
public:
	static constexpr UINT16 s_maxSoundChannels  = 512;
	static constexpr UINT8  s_maxNum3DListeners = 4;

	enum SoundGroup {
		SOUND_GROUP_NONE = 0
	};

public:
	SoundEngine();
	~SoundEngine();

	void CreateSound(std::string name,  UINT mode, FMOD_CREATESOUNDEXINFO* info, FMOD::Sound** sound);
	void CreateStream(std::string name, UINT mode, FMOD_CREATESOUNDEXINFO* info, FMOD::Sound** sound);

	void CreateSoundListener(SoundListener& listener);

	void PlayFSound(FMOD::Sound* sound);

	void UpdateSound(entt::registry& registry, const float dt);

private:
	UINT8 m_num3DListeners = 0;

	FMOD::System* m_soundSystem = nullptr;
};

