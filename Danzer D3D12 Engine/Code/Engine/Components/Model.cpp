#include "stdafx.h"
#include "Model.h"

#ifdef EDITOR_DEBUG_VIEW
#include "imgui/imgui.h"
#include "Core/Engine.h"
#include "Core/D3D12Framework.h"
#include "Core/DesriptorHeapWrapper.h"
#include "Rendering/Models/ModelHandler.h"
#include "Rendering/TextureHandler.h"
#include "FileExplorer.h"
void DisplayModelTexture(uint32_t& texture, const std::string textureType) {
	Engine& eng = Engine::GetInstance();
	CD3DX12_GPU_DESCRIPTOR_HANDLE textureHandle(
		eng.GetFramework().CbvSrvHeap().GET_GPU_DESCRIPTOR(
			eng.GetTextureHandler().GetTextureData(texture).m_offsetID
		)
	);

	if (ImGui::ImageButton(textureType.c_str(), ImTextureID(textureHandle.ptr), { 50.0f, 50.0f })) {
		const std::wstring texturePath = FileExplorer::FetchFileFromExplorer(L"Sprites\\", L".dds");
		if (texturePath != INVALID_FILE_FECTHED && texturePath != L"") {
			texture = eng.GetTextureHandler().GetTexture(texturePath);
		}
	}
	ImGui::SameLine();
	ImGui::Text(textureType.c_str());
}

void Model::DisplayInEditor(const Entity entity)
{
	Model&     model = REGISTRY->Get<Model>(entity);
	Engine&    eng   = Engine::GetInstance();
	
	if (ImGui::Button("Select New Model")) {
		const std::wstring modelPath = FileExplorer::FetchFileFromExplorer(L"Models\\", L".fbx");
		if (modelPath != INVALID_FILE_FECTHED && modelPath != L"") {
			model = eng.GetModelHandler().LoadModel(modelPath); // Jump out of this function so we dont get problems with ModelData
			return;
		}
	}
	
	if (model.m_modelID == UINT32_MAX)
		return;

	ModelData& modelData  = Engine::GetInstance().GetModelHandler().GetLoadedModelInformation(model.m_modelID);
	ImGui::Text(std::string(modelData.GetModelPath().begin(), modelData.GetModelPath().end()).c_str());

	if (!modelData.ModelFinished())
		return;

	const  int32_t meshCount = modelData.GetMeshes().size() - 1;
	static int32_t selectedMesh = 0;
	
	ImGui::DragInt("Selected Mesh", &selectedMesh, 1.0f, 0, meshCount);
	selectedMesh = selectedMesh >= meshCount ? meshCount : selectedMesh;
	selectedMesh = selectedMesh < 0 ? 0 : selectedMesh;
	
	ModelData::Mesh& mesh = modelData.GetSingleMesh(selectedMesh);

	bool renderOnlyMesh = false;
	ImGui::Checkbox("Render Only Selected Mesh", &renderOnlyMesh);
	
	for (uint32_t i = 0; i < meshCount + 1; i++)
	{
		if (i != selectedMesh)
			modelData.GetSingleMesh(i).m_renderMesh = !renderOnlyMesh;
	}
	
	Material& material = mesh.m_material;

	DisplayModelTexture(material.m_albedo,		 "Albedo");
	DisplayModelTexture(material.m_normal,		 "Normal");
	DisplayModelTexture(material.m_metallicMap,  "Metallic Map");
	DisplayModelTexture(material.m_roughnessMap, "Roughness Map");
	DisplayModelTexture(material.m_aoMap,		 "AO Map");
}
#else
void Model::DisplayInEditor(const Entity entity){}
#endif
