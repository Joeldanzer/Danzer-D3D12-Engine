#pragma once
#include <string>

#include "Rendering/Models/ModelLoaderCustom.h"

//* ATTENTION!!!!
//* Keeping this here in case i want to build more on it, otherwise it is kinda abandoned...

//class ModelHandler;
//class TextureHandler;
//
//struct LevelCollider {
//	enum class LVLCOLLIDER {
//		LVL_COLLIDER_NONE,
//		LVL_COLLIDER_AABB,
//		LVL_COLLIDER_SPHERE,
//		LVL_COLLIDER_LINE
//	};
//	LevelCollider() : 
//		m_type(LVLCOLLIDER::LVL_COLLIDER_NONE)
//	{}
//
//	LVLCOLLIDER m_type;
//};
//
////* LevelObject is loaded from assimp and used to load
////* Models and create Scene Objects. If objects LevelLayer
////* is nullptr it is connected to the layer "default"
//struct LevelObject : public Transform{
//	std::string m_tag = "default";
//	std::string m_layer;
//	std::string m_objectName;
//	std::vector<Vect3f> m_verticies;
//	LevelCollider m_collider;
//	UINT m_model;
//};
//
////* Scene Objects located in a specific layer are oriented 
////* in the layers transform. Moving/Rotation/Scaling a layer
////* will affect every object connected to that layer.
//struct LevelLayer : public Transform {
//	~LevelLayer() {
//		for (auto* o : m_levelObjects) { delete o;}
//	}
//	std::vector<LevelObject*> m_levelObjects;
//	std::string m_layerName;
//};
//
////* LevelScene contains information on every layer, model and object
////* the Scene FBX files has. 
//struct LevelScene {
//	//LevelScene();
//	//LevelScene(LevelScene* scene) { *this = scene; };
//	~LevelScene() { for (auto* l : m_levelLayers) { delete l; } }
//	std::vector<LevelLayer*> m_levelLayers;
//	std::vector<UINT>		 m_textureIDs;
//	
//	std::string m_sceneName;
//};
//
//class LevelLoaderCustom
//{
//public:
//	LevelLoaderCustom();
//	LevelLoaderCustom(ModelHandler& modelFactory, TextureHandler& textureHandler);
//	~LevelLoaderCustom();
//
//	std::unique_ptr<LevelScene> LoadLevelFromFBX(std::string fileName);
//private:
//
//	std::vector<std::string> m_tempTextures;
//
//	void CheckForCollider(LevelObject* out, aiNode* child, const aiScene* assimpScene);
//
//	void GetLevelProperites(LevelScene* scene, LevelLayer* layer, aiNode* currentNode, const aiScene* assimpScene, bool transparent = false);
//	LevelObject* CreateLevelObject(LevelLayer* layer, aiNode* child, const aiScene* assimpScene, std::vector<UINT>& textures, bool transparent);
//
//	std::vector<std::string> m_ignoreList;
//	std::vector<std::string> m_transparencyList;
//
//	bool IgnoreObject(std::string name);
//	bool TransparentGroup(std::string name);
//
//	TextureHandler* m_textureHandler;
//	ModelHandler* m_modelHandler;
//	Assimp::Importer m_importer;
//};

