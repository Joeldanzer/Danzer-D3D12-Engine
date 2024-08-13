#pragma once

// General Engine includes 
#include "Core/WindowHandler.h"  
#include "Core/D3D12Framework.h" 
#include "Core/MathDefinitions.h"
#include "Core/D3D12Header.h"

// 3rd Party includes
#include "assimp/config.h"
#include "entt/entt.hpp"
#include "Fmod/fmod.hpp"
#include "Physics/PhysicsHeader.h"

// This needs to be reworked.
enum class EngineState {
	ENGINE_STATE_EDITOR,
	ENGINE_STATE_GAME
} static s_engineState;

