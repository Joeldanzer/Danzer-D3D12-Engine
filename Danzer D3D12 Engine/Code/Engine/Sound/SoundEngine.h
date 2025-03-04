#pragma once

namespace FMOD {
	class System;
	class Stream;
	class Channel;
	class Sound;
}

struct SoundSource;
struct SoundListener;
struct FMOD_CREATESOUNDEXINFO;

class SoundHandler;

typedef uint32_t SOUND_ID;
typedef uint32_t STREAM_ID;

class SoundEngine
{
public:
	static constexpr uint16_t s_maxSoundChannels  = 1024;
	static constexpr uint8_t  s_maxNum3DListeners = 4;

	
	//enum SoundChannel {
	//	SOUND_CHANNEL_NONE = 0,
	//	SOUND_CHANNEL_WORLD = 1,
	//};

public:
	SoundEngine();
	~SoundEngine();

	SOUND_ID  LoadSound( const std::string& soundFile,  bool loop);
	STREAM_ID LoadStream(const std::string& streamFile, bool loop);

	void OLDCreateSound(std::string name,  UINT mode, FMOD_CREATESOUNDEXINFO* info, FMOD::Sound** sound);
	void OLDCreateStream(std::string name, UINT mode, FMOD_CREATESOUNDEXINFO* info, FMOD::Sound** sound);

	void CreateSoundListener(SoundListener& listener);

	void PlaySoundAtEntt( entt::entity entity, SOUND_ID  sound, const float volume = 1.0f, const float pitch = 1.0f);
	void PlayStreamAtEntt(entt::entity entity, STREAM_ID sound, const float volume = 1.0f, const float pitch = 1.0f);

	void UpdateSound(const float dt);

private:
	void RemoveSound(SoundSource& source, const uint32_t index);

	std::string FetchFileName(const std::string& path);

	UINT8 m_num3DListeners = 0;

	std::unordered_map<std::string, SOUND_ID>  m_loadedSounds;
	std::unordered_map<std::string, STREAM_ID> m_loadedStreams;
	
	std::vector<FMOD::Sound*> m_sounds;
	std::vector<FMOD::Sound*> m_streams;

	FMOD::System* m_soundSystem = nullptr;
};

