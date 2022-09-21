#pragma once

#include "../3rdParty/DirectX-Headers-main/include/directx/d3dx12.h"

#include <string>
#include <vector>

class Engine;
class Object;
class Object2D;

class ImguiHandler
{
public:
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
	
	CD3DX12_GPU_DESCRIPTOR_HANDLE AddImguiImage(std::wstring path);

	Engine* m_engine;
	
	std::vector<ImguiTexture> m_imguiTextures;
	// Object Imgui Settings
	
	void Object3DImgui();
	void Object2DImgui();

	Object* m_object;
	float m_pos[3];
	float m_rot[3];
	float m_sca[3];

	Object2D* m_object2D;				
	int m_currentFrame = 0;
	char m_text[1024];
	float m_pos2D[2];
	float m_rot2D[2];

	// Model Imgui Settings
	int m_currentMesh = 0;
	bool m_renderMesh = true;
	//ID3D12Resource* m_copyTexture;

	int m_selectedScene = 1;

};

