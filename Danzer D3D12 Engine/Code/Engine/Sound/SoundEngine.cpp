#include "stdafx.h"
#include "SoundEngine.h"

#include "Components/Sound/SoundListener.h"
#include "Components/Transform.h"

SoundEngine::SoundEngine()
{
	FMOD_RESULT result;
	result = FMOD::System_Create(&m_soundSystem);
	FmodResultCheck(result);

	result = m_soundSystem->init(s_maxSoundChannels, FMOD_INIT_3D_RIGHTHANDED, 0);
	FmodResultCheck(result);

	FmodResultCheck(m_soundSystem->set3DNumListeners(s_maxNum3DListeners));
}

SoundEngine::~SoundEngine()
{
	m_soundSystem->release();
	delete m_soundSystem;
}

void SoundEngine::UpdateSound(entt::registry& reg, const float dt)
{
	auto view = reg.view<SoundListener, Transform>();
	for (auto entity : view)
	{
		SoundListener& listener = reg.get<SoundListener>(entity);
		Transform& transform = reg.get<Transform>(entity);

		Vect3f vel = transform.m_position - transform.LastPosition();
		vel /= dt;

		FMOD_VECTOR fPos	 = { transform.m_position.x, transform.m_position.y, transform.m_position.y };
		FMOD_VECTOR fVel	 = { vel.x, vel.y, vel.z };
		FMOD_VECTOR fUp		 = { transform.World().Up().x, transform.World().Up().y, transform.World().Up().z };
		FMOD_VECTOR fForward = { transform.World().Forward().x, transform.World().Forward().y, transform.World().Forward().z };

		m_soundSystem->set3DListenerAttributes(listener.m_id, &fPos, &fVel, &fForward, &fUp);
	}
}


