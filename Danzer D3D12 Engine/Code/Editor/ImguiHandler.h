#pragma once

#include "DirectX/include/directx/d3dx12.h"
#include "entt/entt.hpp"

#include "Core/MathDefinitions.h"

#include "EditorLoadAndSave.h"
#include "FileExplorer.h"

#include <string>
#include <vector>

class Engine;
struct GameEntity;
struct Object2D;

class ImguiHandler
{
public:
	struct ImGuiWindow {
		UINT m_width;
		UINT m_height;
	
		Vect2f m_positon;
	};

	struct EntityItem {
		//entt::entity m_entity;
	};

	ImguiHandler(Engine& engine);
	~ImguiHandler();

	void Init();

	void Update(const float dt);

private:

	Vect3f m_dirLightRot     =  {0.0f, 0.0f, 0.0f};
	Vect3f m_dirLightLastRot =  {0.0f, 0.0f, 0.0f};
	Quat4f m_dirLightInitQuat = {0.0f, 0.0f, 0.0f, 1.0f};

	Vect3f m_kuwaharaOffset = { 0.0f, 0.0f, 0.0f };
	int m_kuwaharaRadius = 1;
	int m_kuwaharaScale =  1;

	struct ImguiTexture {
		unsigned int m_descriptorIndex;
		std::wstring m_imagePath;
		ID3D12Resource* m_texture;

		CD3DX12_GPU_DESCRIPTOR_HANDLE m_srvGpuHandle;
	};
	
	ImGuiWindow m_rightWindow;
	ImGuiWindow m_leftWindow;

	bool m_itemsHasBeenSelected = false;

	CD3DX12_GPU_DESCRIPTOR_HANDLE AddImguiImage(std::wstring path);

	Engine& m_engine;
	int m_currentMesh = 0;
	//float m_currentRotation[3] ={1.f, 5.f, 30.f};
	entt::entity m_currentEntity;
	bool m_entitySelected;
	char* m_tag;
	char* m_name;

	std::vector<ImguiTexture> m_imguiTextures;
	// Object Imgui Settings
	
	void StaticWindows();

	void SaveScene(entt::registry& reg);
	void SaveSceneAs(entt::registry& reg);

	bool ModelDataSettings(entt::registry& reg);
	bool ObjectSettings(entt::registry& reg);
	bool TransformSettings(entt::registry& reg);
	bool DirectionalLightSettings(entt::registry& reg);

	std::wstring SelectTexture(UINT& texture);

	std::array<std::string, 3> m_stateNames = { "ACTIVE", "NOT_ACTIVE", "DESTROY" };
	std::unordered_map<std::string, FileExplorer::FileType> m_fileExtensions;

	std::vector<std::string> m_componentList;

	EditorLoadAndSave m_sceneLoader;
	FileExplorer m_fileExplorer;

	bool m_removeEntity;
};

