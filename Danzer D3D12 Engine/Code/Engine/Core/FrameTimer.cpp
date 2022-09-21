#include "stdafx.h"
#include "FrameTimer.h"

#include <profileapi.h>

FrameTimer::FrameTimer() {
	QueryPerformanceFrequency(&m_preformanceFrequency);
	QueryPerformanceCounter(&m_lastTick);
}

void FrameTimer::Update() {
	LARGE_INTEGER  currentTick, elapsedTime;
	QueryPerformanceCounter(&currentTick);
	elapsedTime.QuadPart = currentTick.QuadPart - m_lastTick.QuadPart;
	elapsedTime.QuadPart *= 1000000;
	elapsedTime.QuadPart /= m_preformanceFrequency.QuadPart;
	m_deltaTime = (float)(elapsedTime.QuadPart) / 1000000.f;
	m_lastTick = currentTick;
}
