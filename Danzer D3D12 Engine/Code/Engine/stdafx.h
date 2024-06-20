#pragma once

#include "assimp/config.h"
#include "Jolt/Jolt.h"
#include "Fmod/fmod.hpp"

// This needs to be reworked.
enum class EngineState {
	ENGINE_STATE_EDITOR,
	ENGINE_STATE_GAME
} static s_engineState;

