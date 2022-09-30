#pragma once

#include "../3rdParty/DirectX-Headers-main/include/directx/d3dx12.h"
#include "../3rdParty/entt-master/single_include/entt/entt.hpp"
//#include "entt/entt.hpp"
#include "Core/MathDefinitions.h"

#include <string>
#include <vector>

class Engine;
class Object;
class Object2D;


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

	ImguiHandler();
	~ImguiHandler();

	void Init(Engine& engine);

	void Update(const float dt);

private:
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

	Engine* m_engine;
	int m_currentMesh = 0;
	//float m_currentRotation[3] ={1.f, 5.f, 30.f};
	entt::entity m_currentEntity;
	bool m_entitySelected;
	char* m_tag;
	char* m_name;

	std::vector<ImguiTexture> m_imguiTextures;
	// Object Imgui Settings
	
	void StaticWindows();

	bool ModelDataSettings(entt::registry& reg);
	bool ObjectSettings(entt::registry& reg);
	bool TransformSettings(entt::registry& reg);
	bool DirectionalLightSettings(entt::registry& reg);

	// Not sure if I have named it correct
	struct FileExplorerExtension {
		std::wstring m_fileType;
		std::wstring m_folder;
	};

	std::wstring OpenFileExplorer(FileExplorerExtension file);

	std::array<std::string, 3> m_stateNames = { "ACTIVE", "NOT_ACTIVE", "DESTROY" };
	std::unordered_map<std::string, FileExplorerExtension> m_fileExtensions;

	std::vector<std::string> m_componentList;

};

