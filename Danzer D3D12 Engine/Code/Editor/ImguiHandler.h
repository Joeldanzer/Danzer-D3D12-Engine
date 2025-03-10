#pragma once

#include "DirectX/include/directx/d3dx12.h"
#include "ImguiComponentMenus.h"

#include "Core/MathDefinitions.h"

#include "EditorLoadAndSave.h"
#include "FileExplorer.h"

#include <string>
#include <vector>

class Engine;
class Camera;
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
	Vect2f m_lastSceneToWindowSize;
	bool m_sceneViewOpen = true;

	Vect3f m_dirLightRot     =  {0.0f, 0.0f, 0.0f};
	Vect3f m_dirLightLastRot =  {0.0f, 0.0f, 0.0f};

	Vect3f m_kuwaharaOffset = { 0.0f, 0.0f, 0.0f };
	int m_kuwaharaRadius = 1;
	int m_kuwaharaScale =  1;

	struct ImguiTexture {
		unsigned int m_descriptorIndex;
		std::wstring m_imagePath;
		ID3D12Resource* m_texture;

		CD3DX12_GPU_DESCRIPTOR_HANDLE m_srvGpuHandle;
	};

	bool m_itemsHasBeenSelected = false;

	CD3DX12_GPU_DESCRIPTOR_HANDLE AddImguiImage(std::wstring path);

	Engine& m_engine;
	int m_currentMesh = 0;
	//float m_currentRotation[3] ={1.f, 5.f, 30.f};
	entt::entity m_currentEntity;
	bool m_entitySelected;
	char* m_tag;
	char* m_name;

	bool m_renderOnlyMesh = false;

	std::vector<ImguiTexture> m_imguiTextures;
	// Object Imgui Settings
	
	void StaticWindows();

	void SaveScene();
	void SaveSceneAs();

	std::wstring SelectTexture(UINT& texture);

	std::array<std::string, 3> m_stateNames = { "ACTIVE", "NOT_ACTIVE", "DESTROY" };
	std::unordered_map<std::string, FileExplorer::FileType> m_fileExtensions;

	std::vector<std::string> m_componentList;

	std::string m_baseEntityName = "Empty Entity";

	EditorLoadAndSave m_sceneLoader;
	FileExplorer m_fileExplorer;

	bool m_removeEntity;
};

