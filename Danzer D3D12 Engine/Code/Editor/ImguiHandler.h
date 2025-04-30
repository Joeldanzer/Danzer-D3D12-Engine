#pragma once

#include "DirectX/include/directx/d3dx12.h"

#include "Core/MathDefinitions.h"

#include "FileExplorer.h"
#include "entt/entt.hpp"

#include <string>
#include <vector>

struct Camera;
struct Transform;
struct GameEntity;
struct Object2D;

class ImguiHandler
{
public:
	
	ImguiHandler();
	~ImguiHandler();

	void Init();

	void Update(const float dt);

private:
	void SetUpDockingWindows();
	void DrawSceneToWindow(Camera& viewPortCam);
	
	void DisplayViewport();
	void ManipulateGameEntity();

	Vect3f m_dirLightRot     =  {0.0f, 0.0f, 0.0f};
	Vect3f m_dirLightLastRot =  {0.0f, 0.0f, 0.0f};
	Vect3f m_selectedRotation;
	
	Vect2f m_lastSceneToWindowSize;
	Vect2f m_sceneViewSize;
	Vect2f m_sceneViewPosition;
	
	bool m_sceneViewOpen = true;
	bool m_itemsHasBeenSelected = false;

	entt::entity m_currentEntity;
	entt::entity m_previousEntity;
	
	bool  m_entitySelected;

	void StaticWindows();
	void SaveScene();
	void SaveSceneAs();

	std::string  m_baseEntityName    = "Empty Entity";
	std::wstring m_currentSaveTarget = L"";

	bool m_removeEntity;
};

