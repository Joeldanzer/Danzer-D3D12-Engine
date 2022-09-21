#include "stdafx.h"
#include "LevelLoaderCustom.h"

//* ATTENTION!!!
//* Read information in the .hpp file...

//#include <fstream>
//#include <assert.h>
//
//#include "../3rdParty/assimp-master/include/assimp/postprocess.h"
//#include "../3rdParty/assimp-master/include/assimp/scene.h"
//
//#include "Rendering/Models/ModelFactory.h"
//#include "Rendering/TextureHandler.h"
//
//#include <Windows.h>
//
//LevelLoaderCustom::LevelLoaderCustom() :
//    m_modelHandler(nullptr),
//    m_importer()
//{
//}
//
//LevelLoaderCustom::LevelLoaderCustom(ModelHandler& modelFactory, TextureHandler& textureHandler) :
//    m_modelHandler(&modelFactory),
//    m_textureHandler(&textureHandler)
//{
//    // Ignore list for when loadinng in level fbx files
//    m_ignoreList = {
//        "AssimpFbx",
//        "collision",
//        "transform",
//        //"pPlane",
//        "trigger_group",
//        "triggers_group",
//        "triggers_group",
//        "platform_collision",
//        //"screen",
//        "grid_mesh",
//        "mlRoto",
//        "platforms_mesh",
//    };
//
//    m_transparencyList = {
//        "FG_group",
//        "FG_final_group",
//        "BG_group",
//        "FG_objects_group1"
//    };
//}
//
//LevelLoaderCustom::~LevelLoaderCustom(){}
//
//std::unique_ptr<LevelScene> LevelLoaderCustom::LoadLevelFromFBX(std::string fileName)
//{
//    std::ifstream fileExist(fileName);
//    if (!fileExist.good())
//        assert(fileExist, fileName.c_str() + " doesn't exist!");
//    fileExist.close();
//
//    m_importer.SetPropertyBool(AI_CONFIG_FBX_CONVERT_TO_M, true);
//    m_importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, true);
//    //m_importer.SetPropertyBool(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, true);
//    auto flags = 0
//        | aiProcessPreset_TargetRealtime_MaxQuality
//        | aiProcess_ConvertToLeftHanded
//        | aiProcess_GlobalScale
//        | aiProcess_GenBoundingBoxes
//        ;
//
//    const aiScene* scene = m_importer.ReadFile(fileName, flags);
//    assert(scene, "Assimp Failed to load '" + fileName.c_str() + "'");
//
//    aiNode* rootNode = scene->mRootNode;
//
//    // Find parent node
//    while (rootNode->mParent != nullptr)
//        rootNode = rootNode->mParent;
//
//    LevelLayer* defaultLayer = new LevelLayer();
//    defaultLayer->m_layerName = "default";
//    
//    LevelScene* newScene = new LevelScene();
//    newScene->m_levelLayers.emplace_back(defaultLayer);
//
//    // Get all of the textures in the scene now so we dont haft to do it for every object.
//    m_tempTextures = m_modelHandler->GetModelLoader().GetAllTexturesFromScene(scene);
//    newScene->m_textureIDs = m_textureHandler->CreateMultipleTextures(&m_tempTextures[0], static_cast<UINT>(m_tempTextures.size()));
//    GetLevelProperites(newScene, nullptr, rootNode, scene);
//    
//    std::unique_ptr<LevelScene> levelScene = std::make_unique<LevelScene>();
//    levelScene->m_levelLayers = newScene->m_levelLayers;
//    levelScene->m_sceneName = fileName;
//
//	return levelScene;
//}
//
//void LevelLoaderCustom::CheckForCollider(LevelObject* out, aiNode* child, const aiScene* assimpScene)
//{
//    for (UINT i = 0; i < child->mNumMeshes; i++)
//    {
//        std::string meshName(assimpScene->mMeshes[child->mMeshes[i]]->mName.C_Str());
//        if (meshName.find("AABB") != std::string::npos) {
//            aiAABB aabb = assimpScene->mMeshes[child->mMeshes[i]]->mAABB;
//            out->m_collider.m_type = LevelCollider::LVLCOLLIDER::LVL_COLLIDER_AABB;
//            //out->m_tag = "platform";
//            std::string type = child->mName.C_Str();
//            size_t t = type.find_last_of(".");
//            type = type.substr(0, t);
//            out->m_tag = type;
//            break;
//        }
//    }
//  
//}
//
//void LevelLoaderCustom::GetLevelProperites(LevelScene* scene, LevelLayer* layer, aiNode* currentNode, const aiScene* assimpScene, bool transparent)
//{
//    // Exit out of this function if currentNode does not have any children
//    if (!currentNode->mChildren)
//        return;
//
//    ModelLoaderCustom& modelLoader = m_modelHandler->GetModelLoader();
//    for (UINT i = 0; i < currentNode->mNumChildren; i++)
//    {
//        aiNode* child = currentNode->mChildren[i];
//        if (IgnoreObject(child->mName.C_Str()))
//            continue;
//
//        //* if child has children than it is considered as a layer at the moment
//        if (child->mChildren) {
//            if (!layer) {
//                LevelLayer* newLayer = new LevelLayer();
//                newLayer->m_layerName = child->mName.C_Str();
//                newLayer->Translate(modelLoader.ConvertToEngineMat4(child->mTransformation));
//                bool objectsTransparent = TransparentGroup(newLayer->m_layerName);
//                GetLevelProperites(scene, newLayer, child, assimpScene, objectsTransparent);
//                scene->m_levelLayers.emplace_back(newLayer);
//            }
//            else {
//                GetLevelProperites(scene, nullptr, child, assimpScene);
//            }         
//        }
//        
//        //* If child has mesh we create a levelObject with models, positon and collision data
//        if (child->mMeshes) { 
//            if (!layer) 
//                scene->m_levelLayers[0]->m_levelObjects.emplace_back(CreateLevelObject(layer, child, assimpScene, scene->m_textureIDs, transparent));
//            else 
//                layer->m_levelObjects.emplace_back(CreateLevelObject(layer, child, assimpScene, scene->m_textureIDs, transparent));                
//        }
//        //* If child has no mesh than we just create a empty object with a name and a position.
//        //* Layers will also be created as a empty object 
//        else {
//            LevelObject* emptyObject = new LevelObject();
//            emptyObject->Translate(m_modelHandler->GetModelLoader().ConvertToEngineMat4(child->mTransformation));
//            emptyObject->m_objectName = child->mName.C_Str();
//            if(!layer)
//                scene->m_levelLayers[0]->m_levelObjects.emplace_back(emptyObject);
//            else
//                layer->m_levelObjects.emplace_back(emptyObject);
//                
//        }
//    }  
//}
//
//LevelObject* LevelLoaderCustom::CreateLevelObject(LevelLayer* layer, aiNode* child, const aiScene* assimpScene, std::vector<UINT>& textures, bool transparent)
//{
//    LevelObject* object = new LevelObject();
//
//    if (!layer)
//        object->m_layer = "default";
//    else
//        object->m_layer = layer->m_layerName;
//
//    std::string s(child->mName.C_Str());
//
//    CheckForCollider(object, child, assimpScene);
//
//    object->Translate(m_modelHandler->GetModelLoader().ConvertToEngineMat4(child->mTransformation));
//    object->m_objectName = s;
//
//    // Find a better way to create specific lvl object
//    if(object->m_tag != "platform") {
//        object->m_model = m_modelHandler->GetExistingModel(object->m_objectName);
//        if (object->m_model == 0) {
//             std::unique_ptr<LoaderModel> model = m_modelHandler->GetModelLoader().LoadModelFromAiNode(assimpScene, child, textures, true);
//             if (model.get()) {
//                object->m_model = m_modelHandler->LoadModelFromLevel(model.get(), textures, transparent);
//             }
//        }
//    }
//    else
//        object->m_model = 0;
//
//    return object;
//}
//
//bool LevelLoaderCustom::IgnoreObject(std::string name)
//{
//    for (UINT i = 0; i < m_ignoreList.size(); i++)
//    {
//        if (name.find(m_ignoreList[i]) != std::string::npos)
//            return true;
//    }
//    return false;
//}
//
//bool LevelLoaderCustom::TransparentGroup(std::string name)
//{
//    for (UINT i = 0; i < m_transparencyList.size(); i++)
//    {
//        if (name.find(m_transparencyList[i]) != std::string::npos)
//            return true;
//    }
//    return false;
//}

