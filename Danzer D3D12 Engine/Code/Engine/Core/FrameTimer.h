#pragma once
#include <algorithm>
#include <Windows.h>

class FrameTimer
{
public:
	FrameTimer();

	void Update();

	inline float GetRealDeltaTime() const { return m_deltaTime; }
	inline float GetRealFrameRate() const { return 1.f / m_deltaTime; }

	inline float GetCappedDeltaTime() const { return std::min<float>(m_deltaTime, 1.f / m_minFps); }
	inline float GetCappedFrameRate() const { return std::min<float>(1.f / m_deltaTime, m_minFps); }


private:
	LARGE_INTEGER m_preformanceFrequency, m_lastTick;
	float m_deltaTime;
	float m_minFps = 120.0f;
};

