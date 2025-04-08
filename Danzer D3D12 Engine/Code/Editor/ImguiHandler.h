#pragma once

#include "DirectX/include/directx/d3dx12.h"

#include "Core/MathDefinitions.h"

#include "EditorLoadAndSave.h"
#include "FileExplorer.h"

#include <string>
#include <vector>

class Engine;
class Camera;

struct Transform;
struct GameEntity;
struct Object2D;

class ImguiHandler
{
public:
	ImguiHandler(Engine& engine);
	~ImguiHandler();

	void Init();

	void Update(const float dt);

private:
	void SetUpDockingWindows();
	void DrawSceneToWindow(Camera& viewPortCam);
	
	void DisplayViewport();
	void ManipulateGameEntity();
	Vect3f m_selectedRotation;

	Vect2f m_lastSceneToWindowSize;
	Vect2f m_sceneViewSize;
	Vect2f m_sceneViewPosition;
	bool m_sceneViewOpen = true;

	Vect3f m_dirLightRot     =  {0.0f, 0.0f, 0.0f};
	Vect3f m_dirLightLastRot =  {0.0f, 0.0f, 0.0f};

	bool m_itemsHasBeenSelected = false;

	Engine& m_engine;
	int m_currentMesh = 0;

	entt::entity m_currentEntity;
	entt::entity m_previousEntity;
	
	bool m_entitySelected;
	char* m_tag;
	char* m_name;

	void StaticWindows();
	void SaveScene();
	void SaveSceneAs();

	std::array<std::string, 3> m_stateNames = { "ACTIVE", "NOT_ACTIVE", "DESTROY" };
	std::vector<std::string> m_componentList;
	std::string m_baseEntityName = "Empty Entity";

	EditorLoadAndSave m_sceneLoader;
	FileExplorer m_fileExplorer;

	bool m_removeEntity;
};

