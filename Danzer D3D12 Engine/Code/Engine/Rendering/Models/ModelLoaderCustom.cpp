#include "stdafx.h"
#include "ModelLoaderCustom.h"

#include "assimp/Scene.h"
#include "assimp/postprocess.h"

#include <assert.h>
#include <fstream>

ModelLoaderCustom::ModelLoaderCustom(){}
ModelLoaderCustom::~ModelLoaderCustom(){}

std::unique_ptr<LoaderModel> ModelLoaderCustom::LoadModelFromAssimp(std::string fileName, bool uvFlipped)
{
    std::ifstream fileExist(fileName);
    if (!fileExist.good())
        assert(fileExist, fileName.c_str() + " doesn't exist!");

    //AI_CONFIG_COLUM_A
    m_importer.SetPropertyBool(AI_CONFIG_FBX_CONVERT_TO_M, false);
    m_importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, true);
    auto flags = 0
        | aiProcessPreset_TargetRealtime_MaxQuality
        | aiProcess_ConvertToLeftHanded
        | aiProcess_TransformUVCoords
        | aiProcess_CalcTangentSpace
        | aiProcess_GlobalScale
        | aiProcess_FindInstances
        ;

    const aiScene* scene = m_importer.ReadFile(fileName, flags);
    assert(scene, "Assimp Failed to load '" + fileName.c_str() + "'");
    
    std::unique_ptr<LoaderModel> model = std::make_unique<LoaderModel>();
    model->m_scene = scene;

    model->m_name = scene->mName.C_Str();
    model->m_meshes.reserve(scene->mNumMeshes); 

    aiNode* rootNode = scene->mRootNode;
    while (rootNode->mParent != nullptr)
        rootNode = rootNode->mParent;

    GetAllModelProperties(model.get(), rootNode, scene, ConvertToEngineMat4(rootNode->mTransformation), uvFlipped);
    LoadMaterials(scene, model.get());

    return model;
}

std::vector<std::unique_ptr<LoaderModel>> ModelLoaderCustom::LoadMultipleModelsFromAssimp(std::string fileName, std::vector<std::pair<std::string, Mat4f>>& transforms, bool uvFlipped)
{
    std::ifstream fileExist(fileName);
    if (!fileExist.good())
        assert(fileExist, fileName.c_str() + " doesn't exist!");
    //AI_CONFIG_COLUM_A
    m_importer.SetPropertyBool(AI_CONFIG_FBX_CONVERT_TO_M, false);
    m_importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, true);
    auto flags = 0
        | aiProcessPreset_TargetRealtime_MaxQuality
        | aiProcess_ConvertToLeftHanded
        | aiProcess_TransformUVCoords
        | aiProcess_CalcTangentSpace
        | aiProcess_GlobalScale
        | aiProcess_FindInstances
        ;

    const aiScene* scene = m_importer.ReadFile(fileName, flags);
    assert(scene, "Assimp Failed to load '" + fileName.c_str() + "'");

    std::vector<std::unique_ptr<LoaderModel>> models;
    FetchAllModelsInScene(scene, models, transforms, uvFlipped);

    return models;
}

std::unique_ptr<LoaderModel> ModelLoaderCustom::LoadModelFromAiNode(const aiScene* scene, aiNode* node, std::vector<UINT>& textures, bool uvFlipped)
{
    std::unique_ptr<LoaderModel> model = std::make_unique<LoaderModel>();

    model->m_name = node->mName.C_Str();
 
    for (UINT i = 0; i < node->mNumMeshes; i++)
    {
        if (scene->mMeshes[node->mMeshes[i]]) {
            LoaderMesh* mesh = new LoaderMesh();
            aiMesh* assimpMesh = scene->mMeshes[node->mMeshes[i]];

            LoadVerticies(model->m_verticies, assimpMesh, mesh, uvFlipped);
            
            mesh->m_indices.reserve(assimpMesh->mNumFaces * 3LL);
            for  (UINT i = 0; i < assimpMesh->mNumFaces; i++)
            {
                for (UINT j = 0; j < assimpMesh->mFaces[i].mNumIndices; j++)
                {
                    mesh->m_indices.emplace_back(assimpMesh->mFaces[i].mIndices[j]);
                }
            } 
            
            mesh->m_textureIndex = assimpMesh->mMaterialIndex;
            model->m_meshes.emplace_back(mesh);
        }
    }

    if (model->m_meshes.empty())
        return std::unique_ptr<LoaderModel>(nullptr);

    return model;
}

void ModelLoaderCustom::FetchAllModelsInScene(const aiScene* scene, std::vector<std::unique_ptr<LoaderModel>>& models, std::vector<std::pair<std::string, Mat4f>>& transforms, bool uvFlipped)
{
    aiNode* rootNode = scene->mRootNode;
    while (rootNode->mParent != nullptr)
        rootNode = rootNode->mParent;

    for (UINT i = 0; i < rootNode->mNumChildren; i++)
    {
        // First check if we have already created this model.
        std::string modelName = rootNode->mChildren[i]->mName.C_Str();
        Mat4f mat = ConvertToEngineMat4(rootNode->mChildren[i]->mTransformation);// * ConvertToEngineMat4(rootNode->mTransformation);
        // This would work if the model scene was properly made
        //if (CheckModelName(models, modelName)) {
        //    transforms.push_back({modelName, mat });
        //    continue;
        //}
        // still need to Add all the transforms 
        //GetAllModelProperties(models[index].get(), rootNode->mChildren[i], scene, ConvertToEngineMat4(rootNode->mTransformation), uvFlipped);

        models.emplace_back(std::make_unique<LoaderModel>());
        int index = UINT(models.size()) - 1;
        
        models[index]->m_name  = modelName;
        models[index]->m_scene = scene;

        FetchMeshOfChild(scene, rootNode->mChildren[i], Mat4f::Identity, models[index].get(), uvFlipped);
        LoadMaterials(scene, models[index].get());

        transforms.push_back({ modelName, mat });
    }
}

void ModelLoaderCustom::GetAllModelProperties(LoaderModel* out, aiNode* currentNode, const aiScene* scene, Mat4f parentMat, bool uvFlipped)
{
    for (UINT i = 0; i < currentNode->mNumChildren; i++)
    {
        aiNode* child = currentNode->mChildren[i];
        Mat4f   transform = ConvertToEngineMat4(child->mTransformation) * parentMat;

        if (child->mNumChildren > 0)
            GetAllModelProperties(out, child, scene, transform, uvFlipped);

        FetchMeshOfChild(scene, child, transform, out, uvFlipped);
    }
}



// *Loads all the useful vertex information that we need from aiMesh 
void ModelLoaderCustom::LoadVerticies(std::vector<Vect3f>& v3Verts, aiMesh* mesh, LoaderMesh* loaderMesh, bool uvFlipped)
{
    bool position  = mesh->HasPositions();
    bool uv        = mesh->HasTextureCoords(0);
    bool normals   = mesh->HasNormals();
    bool binormTan = mesh->HasTangentsAndBitangents();
    bool bones     = mesh->HasBones();
    bool color     = mesh->HasVertexColors(0);
    
    unsigned int shaderType = 0;

    // Save all the different mesh information into a int, easy to detect if a model should have
    // animations, vertex paint or any other kind of stuff.,,
    shaderType |= position  ? LoaderType_Position  : 0;
    shaderType |= normals   ? LoaderType_Normal    : 0;
    shaderType |= binormTan ? LoaderType_BinormTan : 0;
    shaderType |= color     ? LoaderType_Color     : 0;
    shaderType |= uv        ? LoaderType_UV        : 0;
  //  shaderType |= bones    ? LoaderType_Bones     : 0;

    // Size of our vertex buffer 
    unsigned int vertexBufferSize = 0;
    vertexBufferSize += position ? sizeof(float)  * 4 : 0;
    vertexBufferSize += normals  ? sizeof(float)  * 4 : 0;
    vertexBufferSize += binormTan ? sizeof(float) * 8 : 0;
    vertexBufferSize += color    ? sizeof(float)  * 4 : 0;
    vertexBufferSize += uv       ? sizeof(float)  * 2 : 0;
  //  vertexBufferSize += bones    ? sizeof(float) * 8 : 0;

    loaderMesh->m_shaderType = shaderType; 
    loaderMesh->m_vertexSize = vertexBufferSize;
    loaderMesh->m_vertexCount = mesh->mNumVertices;
    loaderMesh->m_verticies = new char[vertexBufferSize * mesh->mNumVertices];

    // Collect all the necessary vertex data from aiMesh 
    VertexCollector verticies;
    verticies.m_vertexInfo.reserve(mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        if (position){
            verticies.PushVec4({ mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.f });
        }

        if (normals) {
            verticies.PushVec4({ -1.f * mesh->mNormals[i].x, mesh->mNormals[i].y, -1.f * mesh->mNormals[i].z, 1.f });
        }

        if (binormTan) {
            verticies.PushVec4({ -1.f * mesh->mTangents[i].x,    mesh->mTangents[i].y,   -1.f * mesh->mTangents[i].z,   1.f });
            verticies.PushVec4({ -1.f * mesh->mBitangents[i].x,  mesh->mBitangents[i].y, -1.f * mesh->mBitangents[i].z, 1.f });
        }

        if (color) {
            if (mesh->mColors[i])
                verticies.PushVec4({ mesh->mColors[i]->r, mesh->mColors[i]->g, mesh->mColors[i]->b, mesh->mColors[i]->a });
            else
                verticies.PushVec4({ 1.0f, 1.0f, 1.0f, 1.0f });
        }

        if (uv) {
            //if (uvFlipped) 
            //    verticies.PushVec2({ fabs(mesh->mTextureCoords[0][i].x - 1), mesh->mTextureCoords[0][i].y });
            //else   
            verticies.PushVec2({ mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y });
        }
    }

    memcpy(loaderMesh->m_verticies, &verticies.m_vertexInfo[0], vertexBufferSize * mesh->mNumVertices);
}

// *Load all verticies and vertex information with the meshes transform in mind.
void ModelLoaderCustom::LoadVerticiesWithTransform(std::vector<Vect3f>& v3Verts, aiMesh* mesh, LoaderMesh* loaderMesh, Mat4f transform, bool uvFlipped)
{
    bool position  = mesh->HasPositions();
    bool uv        = mesh->HasTextureCoords(0);
    bool normals   = mesh->HasNormals();
    bool binormTan = mesh->HasTangentsAndBitangents();
    bool bones     = mesh->HasBones();
    bool color     = mesh->HasVertexColors(0);
   
    unsigned int shaderType = 0;

    // Save all the different mesh information into a int, easy to detect if a model should have
    // animations, vertex paint or any other kind of stuff.,,
    shaderType |= position  ? LoaderType_Position : 0;
    shaderType |= normals   ? LoaderType_Normal : 0;
    shaderType |= binormTan ? LoaderType_BinormTan : 0;
    shaderType |= color     ? LoaderType_Color : 0;
    shaderType |= uv        ? LoaderType_UV : 0;
    //  shaderType |= bones    ? LoaderType_Bones     : 0;

      // Size of our vertex buffer 
    unsigned int vertexBufferSize = 0;
    vertexBufferSize += position  ? sizeof(float) * 4 : 0;
    vertexBufferSize += normals   ? sizeof(float) * 4 : 0;
    vertexBufferSize += binormTan ? sizeof(float) * 8 : 0;
    vertexBufferSize += color     ? sizeof(float) * 4 : sizeof(float) * 4;
    vertexBufferSize += uv        ? sizeof(float) * 2 : 0;
    //vertexBufferSize += sizeof(float) * 4;
    //  vertexBufferSize += bones    ? sizeof(float) * 8 : 0;

    loaderMesh->m_shaderType = shaderType;
    loaderMesh->m_vertexSize = vertexBufferSize;
    loaderMesh->m_vertexCount = mesh->mNumVertices;
    loaderMesh->m_verticies = new char[vertexBufferSize * mesh->mNumVertices];

    // Collect all the necessary vertex data from aiMesh 
    VertexCollector verticies;
    verticies.m_vertexInfo.reserve(mesh->mNumVertices * vertexBufferSize);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        if (position) {
            Vect4f vertex = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.f };
            vertex = Mul(transform, vertex);
            verticies.PushVec4({ vertex.x, vertex.y, vertex.z, 1.f });
        }

        if (normals) 
            verticies.PushVec4({ mesh->mNormals[i].x,    mesh->mNormals[i].y,     mesh->mNormals[i].z, 1.f });
                                                       
        if (binormTan) {                             
            verticies.PushVec4({ mesh->mTangents[i].x,   mesh->mTangents[i].y,   mesh->mTangents[i].z,   1.f });
            verticies.PushVec4({ mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z, 1.f });
        }

        if (color) {    

               verticies.PushVec4({ 1, 1, 1, 1 });
        }
        else
            verticies.PushVec4({ 1.f, 1.f, 1.f, 0.f });

        if (uv) 
            verticies.PushVec2({ mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y });        
    }

    memcpy(loaderMesh->m_verticies, &verticies.m_vertexInfo[0], vertexBufferSize * mesh->mNumVertices);
}

Mat4f ModelLoaderCustom::ConvertToEngineMat4(const aiMatrix4x4& assimpMatrix)
{
    Mat4f mat;
    
    mat(0, 0) = assimpMatrix.a1; mat(1, 0) = assimpMatrix.a2; mat(2, 0) = assimpMatrix.a3; mat(3, 0) = assimpMatrix.a4;
    mat(0, 1) = assimpMatrix.b1; mat(1, 1) = assimpMatrix.b2; mat(2, 1) = assimpMatrix.b3; mat(3, 1) = assimpMatrix.b4;
    mat(0, 2) = assimpMatrix.c1; mat(1, 2) = assimpMatrix.c2; mat(2, 2) = assimpMatrix.c3; mat(3, 2) = assimpMatrix.c4;
    mat(0, 3) = assimpMatrix.d1; mat(1, 3) = assimpMatrix.d2; mat(2, 3) = assimpMatrix.d3; mat(3, 3) = assimpMatrix.d4;
    return mat;
}

void ModelLoaderCustom::LoadMaterials(const aiScene* scene, LoaderModel* model)
{
    for (unsigned int m = 0; m < model->m_meshes.size(); m++)
    {   
        LoadTexture(aiTextureType_DIFFUSE, model->m_meshes[m]->m_textureIndex, model->m_textures, scene->mMaterials[model->m_meshes[m]->m_textureIndex]); // TEXTURE_DEFINITION_ALBEDO     
        // Will have to fix this some other time :/
        if (!model->m_isTransparent) {
            ai_real opacity = 1.0f;
            aiReturn check = scene->mMaterials[model->m_meshes[m]->m_textureIndex]->Get(AI_MATKEY_OPACITY, opacity);
            if(check != AI_FAILURE)
                model->m_isTransparent = opacity != 1.0f ? true : false;
        }
        //LoadTexture(aiTextureType_UNKNOWN,      model->m_textures, scene->mMaterials[m]); // TEXTURE_DEFINITION_ALBEDO
        //LoadTexture(aiTextureType_SPECULAR,     model->m_textures, scene->mMaterials[m]); // TEXTURE_DEFINITION_ROUGHNESS
        //LoadTexture(aiTextureType_AMBIENT,      model->m_textures, scene->mMaterials[m]); // TEXTURE_DEFINITION_AMBIENTOCCLUSION
        //LoadTexture(aiTextureType_EMISSIVE,     model->m_textures, scene->mMaterials[m]); // TEXTURE_DEFINITION_EMISSIVE
        //LoadTexture(aiTextureType_HEIGHT,       model->m_textures, scene->mMaterials[m]);
        //LoadTexture(aiTextureType_NORMALS,      model->m_textures, scene->mMaterials[m]); // TEXTURE_DEFINITION_NORMAL   
        //LoadTexture(aiTextureType_METALNESS,    model->m_textures, scene->mMaterials[m]); // TEXTURE_DEFINITION_METALNESS
        //LoadTexture(aiTextureType_SHININESS,    model->m_textures, scene->mMaterials[m]);
        //LoadTexture(aiTextureType_OPACITY,      model->m_textures, scene->mMaterials[m]);
        //LoadTexture(aiTextureType_DISPLACEMENT, model->m_textures, scene->mMaterials[m]);
        //LoadTexture(aiTextureType_LIGHTMAP,     model->m_textures, scene->mMaterials[m]);
    }
} 

void ModelLoaderCustom::LoadTexture(int type, int textureIndex, std::vector<std::string>& textures, aiMaterial* material)
{
    int texIndex = 0;
    aiReturn texFound = AI_SUCCESS;

    aiString path; // Filename

    texFound = material->GetTexture((aiTextureType)type, texIndex, &path);
    if (texFound == AI_FAILURE) {
        return;
    }

    std::string filePath = std::string(path.data);
    const size_t lastSlashIdx = filePath.find_last_of("\\/");
    if (std::string::npos != lastSlashIdx) {
        filePath.erase(0, lastSlashIdx + 1);
        const size_t replaceTextureType = filePath.find("_Diffuse");
        
        if (filePath.find("Fabric") != std::string::npos) { // Dum if check since the naming convention of this model is horrible
            filePath.erase(replaceTextureType, 8);
            const size_t removeExtension = filePath.find_first_of(".");
            filePath.erase(filePath.begin() + removeExtension, filePath.end());
        } 
        else
            filePath.erase(replaceTextureType, filePath.size() - replaceTextureType);
       
        filePath.insert(0, "Sprites/");
    }

    //float opacity = 1.0f;
    //aiReturn check = material->Get(AI_MATKEY_OPACITY, opacity);

    textures.push_back(filePath);
}

std::string ModelLoaderCustom::FixModelName(std::string name){
    std::string newName = "";
    for (char& c : name)
    {
        if (!std::isdigit(c)) {
            newName += c;
        }
    }

    return newName;
}

void ModelLoaderCustom::LoadTexture(int type, std::vector<std::string>& textures, aiMaterial* material, const aiScene* scene)
{
    aiReturn texFound = AI_SUCCESS;

    aiString path; // Filename

    texFound = material->GetTexture((aiTextureType)type, 0, &path);
    if (texFound == AI_FAILURE) {
        return;
    }

    std::string filePath = std::string(path.data);
    const size_t lastSlashIdx = filePath.find_last_of("\\/");
    if (std::string::npos != lastSlashIdx) {
        filePath.erase(0, lastSlashIdx + 1);
    }
    
    textures.emplace_back(filePath);
}

void ModelLoaderCustom::FetchMeshOfChild(const aiScene* scene, aiNode* child, const Mat4f& transform, LoaderModel* out, bool uvFlipped)
{
    for (UINT i = 0; i < child->mNumMeshes; i++)
    {
        aiMesh* assimpMesh = scene->mMeshes[child->mMeshes[i]];
        LoaderMesh* mesh = new LoaderMesh();

        mesh->m_name = assimpMesh->mName.C_Str();

        mesh->m_textureIndex = assimpMesh->mMaterialIndex;
        LoadVerticiesWithTransform(out->m_verticies, assimpMesh, mesh, transform, uvFlipped);

        mesh->m_indices.reserve(assimpMesh->mNumFaces * 3LL);
        for (UINT p = 0; p < assimpMesh->mNumFaces; p++)
            for (UINT k = 0; k < assimpMesh->mFaces[p].mNumIndices; k++)
                mesh->m_indices.emplace_back(assimpMesh->mFaces[p].mIndices[k]);

        out->m_meshes.emplace_back(mesh);
        int s = 1;
    }
}

bool ModelLoaderCustom::CheckModelName(std::vector<std::unique_ptr<LoaderModel>>& models, std::string& name)
{
    // First check if we have already created this model.
    name.erase(std::remove_if(name.begin(), name.end(), ::isdigit), name.end());

    for (int i = 0; i < models.size(); i++)
        if (models[i]->m_name == name)
            return true;

    return false;
}

std::vector<std::string> ModelLoaderCustom::GetAllTexturesFromScene(const aiScene* scene)
{
   std::vector<std::string> textures;

   for (UINT m = 0; m < scene->mNumMaterials; m++)
   {         
       // 21 = Number of aiTextureTypes
       for (UINT i = 1; i < 21; i++)
       {
            LoadTexture(i, textures, scene->mMaterials[m], scene); 
       }
   }

    return textures;
}
