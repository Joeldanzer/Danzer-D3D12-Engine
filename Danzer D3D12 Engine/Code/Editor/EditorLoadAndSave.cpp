#include "EditorLoadAndSave.h"

#include "AllComponents.h"

#include "Core/Engine.h"
#include "Rendering/Models/ModelHandler.h"
#include "Rendering/TextureHandler.h"

#include "Rendering/Camera.h"

#include <fstream>

EditorLoadAndSave::EditorLoadAndSave(Engine& engine) :
	m_engine(engine),
	m_currentScene("")
{
}
EditorLoadAndSave::~EditorLoadAndSave()
{
}

bool EditorLoadAndSave::LoadScene(std::string scene, entt::registry& reg)
{
	m_currentScene = scene;

	bool success = false;

	rapidjson::Document doc;
	success = ReadJsonDocument(scene, doc);

	rapidjson::Value& sceneEntities = doc["3D-Scene-Entities"];
	for (rapidjson::SizeType i = 0; i < sceneEntities.Size(); i++)
	{
		auto& object = sceneEntities[i];
		
		if (object.IsObject()) {
			entt::entity entity = reg.create();

			auto& objectData = object["Object"];
			Object& obj = reg.emplace<Object>(entity);

			obj.m_name = objectData["Name"].GetString();
			obj.m_layer = (UINT)objectData["Layer"].GetInt();
			obj.m_tag = objectData["Tag"].GetString();
			obj.m_static = objectData["Static"].GetBool();
			
			if (objectData["State"] == "ACTIVE") 
				obj.m_state = Object::STATE::ACTIVE;
			else if (objectData["State"] == "NOT_ACTIVE") 
				obj.m_state = Object::STATE::NOT_ACTIVE;
			else
				obj.m_state = Object::STATE::NOT_ACTIVE;

			auto& transformData = object["Transform"];
			Transform& transform = reg.emplace<Transform>(entity);

			auto pos = transformData["Position"].GetArray();
			transform.m_position = {
				(float)pos[0].GetDouble(), 
				(float)pos[1].GetDouble(), 
				(float)pos[2].GetDouble()
			};
			
			auto rot = transformData["Rotation"].GetArray();
			Vect3f euler = { 
				ToRadians((float)rot[0].GetDouble()),
				ToRadians((float)rot[1].GetDouble()),
				ToRadians((float)rot[2].GetDouble()) 
			};
			
			transform.m_rotation = Quat4f::CreateFromYawPitchRoll(euler);

			auto scale = transformData["Scale"].GetArray();
			transform.m_scale = {
				(float)scale[0].GetDouble(),
				(float)scale[1].GetDouble(),
				(float)scale[2].GetDouble()
			};

			GetDirectionalLightSettings(entity, reg, object);
			GetModelSettings(entity, reg, object);
		}	
	}

	return success;
}

void EditorLoadAndSave::SaveScene(std::string scene, entt::registry& reg)
{
	m_currentScene = scene;

	rapidjson::StringBuffer s;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
	
	writer.StartObject();
	{
		writer.Key("3D-Scene-Entities");
		writer.StartArray();
		auto list = reg.view<Transform, Object>();
		for (auto entity : list)
		{
			if (reg.try_get<Camera>(entity)) {
				continue;
			}

			writer.StartObject();
			{
				Object& obj = reg.get<Object>(entity);
				
				writer.Key("Object");
				writer.StartObject(); 
				{
					writer.Key("Name");
					writer.String(obj.m_name.c_str());
					
					writer.Key("Layer");
					writer.Int(obj.m_layer);
					
					writer.Key("Tag");
					writer.String(obj.m_tag.c_str());

					writer.Key("Static");
					writer.Bool(obj.m_static);

					writer.Key("State");
					switch (obj.m_state)
					{
					case Object::STATE::ACTIVE:
						writer.String("ACTIVE");
						break;
					case Object::STATE::NOT_ACTIVE:
						writer.String("NOT_ACTIVE");
						break;
					default:
						writer.String("NOT_ACTIVE");
						break;
					}
				}
				writer.EndObject();

				Transform& transform = reg.get<Transform>(entity); 
				Vect3f euler = transform.m_rotation.ToEuler();
				euler = { ToDegrees(euler.x), ToDegrees(euler.y), ToDegrees(euler.z) };

				writer.Key("Transform");
				writer.StartObject();
				{
					writer.Key("Position");
					writer.StartArray(); 
					{
						writer.Double(transform.m_position.x);
						writer.Double(transform.m_position.y);
						writer.Double(transform.m_position.z);

					}
					writer.EndArray();

					writer.Key("Rotation");
					writer.StartArray();
					{
						writer.Double(euler.x);
						writer.Double(euler.y);
						writer.Double(euler.z);

					}
					writer.EndArray();

					writer.Key("Scale");
					writer.StartArray();
					{
						writer.Double(transform.m_scale.x);
						writer.Double(transform.m_scale.y);
						writer.Double(transform.m_scale.z);

					}
					writer.EndArray();	
				}
				writer.EndObject();

				DirectionalLight* dirLight = reg.try_get<DirectionalLight>(entity);
				if(dirLight)
					SaveDirectionalLight(dirLight, writer);

				Model* model = reg.try_get<Model>(entity);
				if(model)
					SaveModelSettings(model, writer);

			}
			writer.EndObject();
		}
		writer.EndArray();

	}
	writer.EndObject();

	//* Write to file
	std::ofstream of(scene);
	if (of.is_open()) {
		of << s.GetString();
		of.close();
	}
}

void EditorLoadAndSave::SaveModelSettings(Model* model, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	ModelData& data = m_engine.GetModelHandler().GetLoadedModelInformation(model->m_modelID);

	writer.Key("Model");
	writer.StartObject();
	{
		writer.Key("File");
		std::string path(data.GetModelPath().begin(), data.GetModelPath().end());
		writer.String(path.c_str());

		writer.Key("Materials");
		writer.StartArray();
		std::vector<ModelData::Mesh>& meshes = data.GetMeshes();
		for (UINT i = 0; i < meshes.size(); i++)
		{
			Material& material = meshes[i].m_material;
			writer.StartObject();
			{
				writer.Key("Roughness");
				writer.Double(material.m_roughness);

				writer.Key("Metallic");
				writer.Double(material.m_shininess);

				writer.Key("Emissive");
				writer.Double(material.m_emissvie);

				writer.Key("Color");
				writer.StartArray();
				{
					writer.Double(material.m_color[0]);
					writer.Double(material.m_color[1]);
					writer.Double(material.m_color[2]);
				}
				writer.EndArray();

				writer.Key("Albedo");
				std::wstring albedo = m_engine.GetTextureHandler().GetTextureData(material.m_albedo).m_texturePath;
				std::string str(albedo.begin(), albedo.end());
				writer.String(str.c_str());

				writer.Key("Normal");
				std::wstring normal = m_engine.GetTextureHandler().GetTextureData(material.m_normal).m_texturePath;
				str = { normal.begin(), normal.end() };
				writer.String(str.c_str());

				writer.Key("Metallic Map");
				std::wstring met = m_engine.GetTextureHandler().GetTextureData(material.m_metallicMap).m_texturePath;
				str = { met.begin(), met.end() };
				writer.String(str.c_str());

				writer.Key("Roughness Map");
				std::wstring rog = m_engine.GetTextureHandler().GetTextureData(material.m_roughnessMap).m_texturePath;
				str = { rog.begin(), rog.end() };
				writer.String(str.c_str());

				writer.Key("Height Map");
				std::wstring h = m_engine.GetTextureHandler().GetTextureData(material.m_heightMap).m_texturePath;
				str = { h.begin(), h.end() };
				writer.String(str.c_str());

				writer.Key("AO Map");
				std::wstring ao = m_engine.GetTextureHandler().GetTextureData(material.m_aoMap).m_texturePath;
				str = { ao.begin(), ao.end() };
				writer.String(str.c_str());


			}
			writer.EndObject();
		}
		writer.EndArray();
	}
	writer.EndObject();
}

void EditorLoadAndSave::GetModelSettings(entt::entity entity, entt::registry& reg, rapidjson::Value& value)
{
	if (value.HasMember("Model")) {
		auto& modelData = value["Model"];
		std::string s = modelData["File"].GetString();
		std::wstring path(s.begin(), s.end());
		
		Model& model = reg.emplace<Model>(entity, m_engine.GetModelHandler().LoadModel(path));

		rapidjson::Value& materialArray = modelData["Materials"];
		for (rapidjson::SizeType i = 0; i < materialArray.Size(); i++) {
			
			auto& meshMaterial = materialArray[i];
			if (meshMaterial.IsObject()) {
				float roughness = (float)meshMaterial["Roughness"].GetDouble();
				float metallic  = (float)meshMaterial["Metallic"].GetDouble();
				float emissive  = (float)meshMaterial["Emissive"].GetDouble();

				auto& colorArray = meshMaterial["Color"];

				float color[4] = {
					(float)colorArray[0].GetDouble(),
					(float)colorArray[1].GetDouble(),
					(float)colorArray[2].GetDouble(),
					1.f
				};

				std::string textures[6] = {
					meshMaterial["Albedo"].GetString(),
					meshMaterial["Normal"].GetString(),
					meshMaterial["Metallic Map"].GetString(),
					meshMaterial["Roughness Map"].GetString(),
					meshMaterial["Height Map"].GetString(),
					meshMaterial["AO Map"].GetString(),
				};
				Material material = m_engine.GetTextureHandler().CreateMaterial(textures, metallic, roughness, emissive, color);
				m_engine.GetModelHandler().SetMaterialForModel(model.m_modelID, material, (UINT)i);
			}

		}
	}
}

void EditorLoadAndSave::SaveDirectionalLight(DirectionalLight* dirLight, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{

	writer.Key("Directional Light");
	writer.StartObject();
	{
		writer.Key("Light Color");
		writer.StartArray();
		{
			writer.Double(dirLight->m_lightColor.x);
			writer.Double(dirLight->m_lightColor.y);
			writer.Double(dirLight->m_lightColor.z);
		}
		writer.EndArray();
		
		writer.Key("Light Strength");
		writer.Double(dirLight->m_lightColor.w);

		writer.Key("Ambient Color");
		writer.StartArray();
		{
			writer.Double(dirLight->m_ambientColor.x);
			writer.Double(dirLight->m_ambientColor.y);
			writer.Double(dirLight->m_ambientColor.z);
		}
		writer.EndArray();

		writer.Key("Ambient Strength");
		writer.Double(dirLight->m_ambientColor.w);
	}
	writer.EndObject();
}

void EditorLoadAndSave::GetDirectionalLightSettings(entt::entity entity, entt::registry& reg, rapidjson::Value& value)
{
	if (value.HasMember("Directional Light")) {
		auto& dirLightData = value["Directional Light"];

		DirectionalLight& dirLight = reg.emplace<DirectionalLight>(entity);
		dirLight.m_ambientColor.w = (float)dirLightData["Ambient Strength"].GetDouble();


		auto& lightColor = dirLightData["Light Color"];
		dirLight.m_lightColor = {
			(float)lightColor[0].GetDouble(),
			(float)lightColor[1].GetDouble(),
			(float)lightColor[2].GetDouble(),
			(float)dirLightData["Light Strength"].GetDouble()
		};

		auto& ambientColor = dirLightData["Ambient Color"];
		dirLight.m_ambientColor = {
			(float)ambientColor[0].GetDouble(),
			(float)ambientColor[1].GetDouble(),
			(float)ambientColor[2].GetDouble(),
			(float)dirLightData["Ambient Strength"].GetDouble()
		};
	}
}

bool EditorLoadAndSave::ReadJsonDocument(const std::string& scene, rapidjson::Document& doc)
{
	char* str = nullptr;
	
	{
		FILE* f = fopen(scene.c_str(), "rb");
		if (f) {
			fseek(f, 0, SEEK_END);
			size_t length = ftell(f);
			str = (char*)malloc(length + 1);
			if (str) {
				rewind(f);
				fread(str, sizeof(char), length, f);
				str[length] = '\0';

				fclose(f);
			}
			else
				return false;
		}
		else
			return false;

		doc.Parse(str);
	}

	free(str);

	return true;
}
