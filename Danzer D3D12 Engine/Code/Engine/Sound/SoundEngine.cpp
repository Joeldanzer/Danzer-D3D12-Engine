#include "stdafx.h"
#include "SoundEngine.h"
#include "Components/Sound/SoundListener.h"

#include "Components/Transform.h"
#include "Components/GameEntity.h"
#include "Components/Sound/SoundSource.h"
#include "Components/Sound/SoundListener.h"

using namespace FMOD;

SoundEngine::SoundEngine()
{
	FMOD_RESULT result;
	result = System_Create(&m_soundSystem);
	FmodResultCheck(result);

	result = m_soundSystem->init(s_maxSoundChannels, FMOD_INIT_3D_RIGHTHANDED, 0);
	FmodResultCheck(result);

	FmodResultCheck(m_soundSystem->set3DNumListeners(s_maxNum3DListeners));

}
SoundEngine::~SoundEngine()
{
	for (UINT i = 0; i < m_sounds.size(); i++)
		FmodResultCheck(m_sounds[i]->release());
	m_sounds.clear();

	for (UINT i = 0; i < m_streams.size(); i++)
		FmodResultCheck(m_streams[i]->release());
	m_streams.clear();

	FmodResultCheck(m_soundSystem->release());
	delete m_soundSystem;
}

SOUND_ID SoundEngine::LoadSound(const std::string& soundFile, bool loop)
{
	std::string name = FetchFileName(soundFile);
	if (m_loadedSounds.find(name) != m_loadedSounds.end()) {
		return m_loadedSounds[name];
	}

	FMOD::Sound* sound;
	FmodResultCheck(m_soundSystem->createSound(soundFile.c_str(), FMOD_DEFAULT, nullptr, &sound));

	FmodResultCheck(sound->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF));

	m_sounds.push_back(sound);
	m_loadedSounds.insert({ name, m_sounds.size() - 1 });

	return m_loadedSounds[name];
}

STREAM_ID SoundEngine::LoadStream(const std::string& streamFile, bool loop)
{
	return UINT_MAX;
}

void SoundEngine::OLDCreateSound(std::string name, UINT mode, FMOD_CREATESOUNDEXINFO* info, FMOD::Sound** sound)
{
	FmodResultCheck(m_soundSystem->createSound(name.c_str(), mode, info, sound));
}

void SoundEngine::OLDCreateStream(std::string name, UINT mode, FMOD_CREATESOUNDEXINFO* info, FMOD::Sound** sound)
{
	FmodResultCheck(m_soundSystem->createStream(name.c_str(), mode, info, sound));
}

void SoundEngine::CreateSoundListener(SoundListener& listener)
{
	if (m_num3DListeners < s_maxNum3DListeners) {
		listener.m_id = m_num3DListeners;
		m_num3DListeners++;
	}
#ifdef DEBUG
	else
		std::cout << "Max Number of 3D Listeners has exceeded '" << s_maxNum3DListeners << "'!" << std::endl;
#endif
	
}

void SoundEngine::PlaySoundAtEntt(entt::entity entity, SOUND_ID sound, const float volume, const float pitch)
{
	if (sound == UINT_MAX) {
		std::cout << "Sound with ID: " + std::to_string(sound) + "' does not exist!" << std::endl;
		return;
	}

	Transform&   transform   = Reg::Instance()->Get<Transform>(entity);
	SoundSource& soundSource = Reg::Instance()->Get<SoundSource>(entity);

	Channel* newChannel = nullptr;
	FmodResultCheck(m_soundSystem->playSound(m_sounds[sound], nullptr, true, &newChannel));
	soundSource.m_channels.push_back(newChannel);

	FMOD_VECTOR vec = { transform.m_position.x, transform.m_position.y, transform.m_position.z };

	newChannel->setMode(FMOD_3D);
	newChannel->set3DAttributes(&vec, nullptr);
	newChannel->setVolume(volume < 0.0f ? 1.0f : volume);
	newChannel->setPitch(pitch);
	newChannel->setPaused(false);
}

void SoundEngine::PlayStreamAtEntt(entt::entity entity, STREAM_ID sound, const float volume, const float pitch)
{
	if (sound == UINT_MAX) {
		std::cout << "Sound with ID: " + std::to_string(sound) + "' does not exist!" << std::endl;
		return;
	}

	Transform&   transform   = Reg::Instance()->Get<Transform>(entity);
	SoundSource& soundSource = Reg::Instance()->Get<SoundSource>(entity);

	Channel* newChannel = nullptr;
	FmodResultCheck(m_soundSystem->playSound(m_sounds[sound], nullptr, true, &newChannel));
	soundSource.m_channels.push_back(newChannel);

	FMOD_VECTOR vec = { transform.m_position.x, transform.m_position.y, transform.m_position.z };

	newChannel->setMode(FMOD_3D);
	newChannel->set3DAttributes(&vec, nullptr);
	newChannel->setVolume(volume < 0.0f ? 1.0f : volume);
	newChannel->setPitch(pitch);
	newChannel->setPaused(false);
}

void SoundEngine::UpdateSound(const float dt)
{
	auto listenerView = Reg::Instance()->GetRegistry().view<SoundListener, Transform, GameEntity>();
	for (auto entity : listenerView)
	{	
		SoundListener& listener = Reg::Instance()->Get<SoundListener>(entity);
		if (listener.m_id == s_invalidListener)
			continue;

		Transform& transform = Reg::Instance()->Get<Transform>(entity);

		Vect3f vel = transform.m_position - listener.m_lastPosition;
		vel /= dt;

		FMOD_VECTOR fPos	 = { transform.m_position.x, transform.m_position.y, transform.m_position.y };
		FMOD_VECTOR fVel	 = { vel.x, vel.y, vel.z };
		FMOD_VECTOR fUp		 = { transform.World().Up().x, transform.World().Up().y, transform.World().Up().z };
		FMOD_VECTOR fForward = { transform.World().Forward().x, transform.World().Forward().y, transform.World().Forward().z };

		listener.m_lastPosition = transform.m_position;

		FmodResultCheck(m_soundSystem->set3DListenerAttributes(listener.m_id, &fPos, &fVel, &fForward, &fUp));
	}

	auto sourceView = Reg::Instance()->GetRegistry().view<SoundSource, Transform, GameEntity>();
	bool isPlaying = false;
	for (auto entity : sourceView )
	{
		SoundSource& source  = Reg::Instance()->Get<SoundSource>(entity);
		Transform& transform = Reg::Instance()->Get<Transform>(entity);

		Vect3f velocity = transform.m_position - source.m_lastPosition;
		velocity /= dt;

		FMOD_VECTOR pos = { transform.m_position.x, transform.m_position.y,	transform.m_position.z };
		FMOD_VECTOR vel = { velocity.x, velocity.y, velocity.z};

		source.m_lastPosition = transform.m_position;

		for (uint32_t i = 0; i < source.m_channels.size(); i++)
		{
			isPlaying = false;
			source.m_channels[i]->isPlaying(&isPlaying);
			if (isPlaying)
				FmodResultCheck(source.m_channels[i]->set3DAttributes(&pos, &vel));
			else
				RemoveSound(source, i);
		}
		
	}
	
	FmodResultCheck(m_soundSystem->update());

}

void SoundEngine::RemoveSound(SoundSource& source, const uint32_t index)
{
	if (source.m_channels.size() > 1) {
		source.m_channels[index] = source.m_channels[source.m_channels.size() - 1];
		source.m_channels.pop_back();
	}
	else
		source.m_channels.pop_back();
}

std::string SoundEngine::FetchFileName(const std::string& path)
{
	std::string name = path.substr(path.find_last_of("/") + 1);
	name = path.substr(0, name.find_first_of("."));
	return name;
}


