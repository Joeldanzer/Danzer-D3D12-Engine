#include "EditorLoadAndSave.h"

#include "Components/AllComponents.h"

#include "Core/Engine.h"
#include "Rendering/Models/ModelHandler.h"
#include "Rendering/TextureHandler.h"

#include "Rendering/Camera.h"

#include <fstream>

EditorLoadAndSave::EditorLoadAndSave(Engine& engine) :
	m_engine(engine)
{
}
EditorLoadAndSave::~EditorLoadAndSave()
{
}

