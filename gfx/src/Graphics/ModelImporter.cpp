#include "ModelImporter.hpp"
#include "Vertex.hpp"
#include "Mesh.hpp"
#include "MeshRenderer.hpp"
#include "Texture2D.hpp"
#include "Image.hpp"
#include "Materials/DiffuseMaterial.hpp"
#include "../Core/Resources.hpp"
#include "../Core/Debug.hpp"
#include "../Core/Constants.hpp"
#include "../System/IO/File.hpp"
#include "../../libs/assimp/include/assimp/Importer.hpp"
#include "../../libs/assimp/include/assimp/scene.h"
#include "../../libs/assimp/include/assimp/postprocess.h"
#include "../../libs/assimp/include/assimp/material.h"
#include "../../libs/assimp/include/assimp/texture.h"
#include <iostream>
#include <filesystem>

namespace GFX
{    
    static Matrix4 ToMatrix4(aiMatrix4x4 matrix)
    {
        matrix.Transpose();
        Matrix4 mat;
        mat[0][0] = matrix[0][0];
        mat[0][1] = matrix[0][1];
        mat[0][2] = matrix[0][2];
        mat[0][3] = matrix[0][3];

        mat[1][0] = matrix[1][0];
        mat[1][1] = matrix[1][1];
        mat[1][2] = matrix[1][2];
        mat[1][3] = matrix[1][3];

        mat[2][0] = matrix[2][0];
        mat[2][1] = matrix[2][1];
        mat[2][2] = matrix[2][2];
        mat[2][3] = matrix[2][3];

        mat[3][0] = matrix[3][0];
        mat[3][1] = matrix[3][1];
        mat[3][2] = matrix[3][2];
        mat[3][3] = matrix[3][3];
        return mat;
    }

    static void DumpTextureNames(aiMaterial *aMaterial)
    {
        aiTextureType types[19] = {
            aiTextureType_NONE,
            aiTextureType_DIFFUSE,
            aiTextureType_SPECULAR,
            aiTextureType_AMBIENT,
            aiTextureType_EMISSIVE,
            aiTextureType_HEIGHT,
            aiTextureType_NORMALS,
            aiTextureType_SHININESS,
            aiTextureType_OPACITY,
            aiTextureType_DISPLACEMENT,
            aiTextureType_LIGHTMAP,
            aiTextureType_REFLECTION,
            aiTextureType_BASE_COLOR,
            aiTextureType_NORMAL_CAMERA,
            aiTextureType_EMISSION_COLOR,
            aiTextureType_METALNESS,
            aiTextureType_DIFFUSE_ROUGHNESS,
            aiTextureType_AMBIENT_OCCLUSION,
            aiTextureType_UNKNOWN
        };

        for(size_t i = 0; i < 19; i++)
        {
            uint32_t textureCount = aMaterial->GetTextureCount(types[i]);

            if(textureCount > 0)
                printf("Texture count %zu\n", textureCount);

            for(uint32_t j = 0; j < textureCount; j++)
            {
                aiString path;
                if (aMaterial->GetTexture(types[i], j, &path) == AI_SUCCESS) 
                {
                    std::cout << " Texture " << j << ": " << path.C_Str() << std::endl;
                }
            }
        }
    }

    static Texture2D *LoadDiffuseTexture(aiMaterial *aMaterial, const std::string modelBaseDirectory)
    {
        uint32_t count = aMaterial->GetTextureCount(aiTextureType_DIFFUSE);
        
        if(count == 0)
            return nullptr;
        
        aiString path;

        if (aMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path) != AI_SUCCESS) 
            return nullptr;
        
        std::string filepath = modelBaseDirectory + "/" + std::string(path.C_Str());

        //Try to load file from path
        if(!std::filesystem::exists(filepath))
        {
            //If path doesn't exist, fall back to relative folder in model directory
            filepath = modelBaseDirectory + "/Textures/" + File::GetName( std::string(path.C_Str()));

            if(!std::filesystem::exists(filepath))
                return nullptr;
        }

        //Check if texture already exists
        Texture2D *texture = Resources::FindTexture2D(filepath);
        
        if(texture)
            return texture;

        Image image(filepath);
        
        if(!image.IsLoaded())
            return nullptr;
        
        return Resources::AddTexture2D(filepath, Texture2D(&image));
    }

    GameObject *ModelImporter::LoadFromFile(const std::string &filepath, ModelFlags modelFlags, const Vector3 &scale, bool flipYZ)
    {
		if(!std::filesystem::exists(filepath))
        {
            Debug::WriteLog("Error loading model: file does not exist: %s", filepath.c_str());
            return nullptr;
        }

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filepath, modelFlags);

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            Debug::WriteLog("Error loading model: %s", importer.GetErrorString());
            return nullptr;
        }

        GameObject *model = GameObject::Create();

        File file(filepath);

        ProcessNode(model, scene->mRootNode, scene, scale, flipYZ, file.GetDirectoryPath());

        return model;
    }

    GameObject *ModelImporter::LoadFromMemory(const void *memory, size_t size, ModelFlags modelFlags, const Vector3 &scale, bool flipYZ)
    {
        if(size == 0)
        {
            Debug::WriteLog("Error loading model: size is 0");
            return nullptr;
        }

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFileFromMemory(memory, size, modelFlags);

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            Debug::WriteLog("Error loading model: %s", importer.GetErrorString());
            return nullptr;
        }

        GameObject *model = GameObject::Create();

        ProcessNode(model, scene->mRootNode, scene, scale, flipYZ, "");

        return model;
    }

    std::vector<std::shared_ptr<Mesh>> ModelImporter::LoadMeshesFromFile(const std::string &filepath, ModelFlags modelFlags, const Vector3 &scale, bool flipYZ)
    {
        std::vector<std::shared_ptr<Mesh>> meshes;

        if(!std::filesystem::exists(filepath))
        {
            Debug::WriteLog("Error loading mesh: file does not exist: %s", filepath.c_str());
            return meshes;
        }

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filepath, modelFlags);

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            Debug::WriteLog("Error loading mesh: %s", importer.GetErrorString());
            return meshes;
        }

        for(size_t i = 0; i < scene->mNumMeshes; i++)
        {
            aiMesh * aMesh = scene->mMeshes[i];
            std::vector<Vertex> vertices;
            vertices.resize(aMesh->mNumVertices);

            if(aMesh->HasTextureCoords(0))
            {
                for(size_t j = 0; j < aMesh->mNumVertices; j++)
                {
                    auto pos = aMesh->mVertices[j];
                    auto nrm = aMesh->mNormals[j];
                    auto uv = aMesh->mTextureCoords[0][j];

                    if(flipYZ)
                    {
                        float y = pos.z;
                        float z = pos.y;
                        pos.y = y;
                        pos.z = z;
                    }

                    vertices[j].position = Vector3(pos.x, pos.y, pos.z) * scale;
                    vertices[j].normal = Vector3(nrm.x, nrm.y, nrm.z);
                    vertices[j].uv = Vector2(uv.x, uv.y);
                }
            }
            else
            {
                for(size_t j = 0; j < aMesh->mNumVertices; j++)
                {
                    auto pos = aMesh->mVertices[j];
                    auto nrm = aMesh->mNormals[j];

                    if(flipYZ)
                    {
                        float y = pos.z;
                        float z = pos.y;
                        pos.y = y;
                        pos.z = z;
                    }

                    vertices[j].position = Vector3(pos.x, pos.y, pos.z) * scale;
                    vertices[j].normal = Vector3(nrm.x, nrm.y, nrm.z);
                    vertices[j].uv = Vector2(0.0f, 0.0f);
                }
            }

            std::vector<GLuint> indices;
            indices.resize(aMesh->mNumFaces * 3);
            size_t index = 0;

            for(size_t j = 0; j < aMesh->mNumFaces; j++)
            {
                indices[index+0] = aMesh->mFaces[j].mIndices[0];
                indices[index+1] = aMesh->mFaces[j].mIndices[1];
                indices[index+2] = aMesh->mFaces[j].mIndices[2];
                index += 3;
            }

            auto mesh = std::make_shared<Mesh>(vertices, indices, false);
            meshes.push_back(mesh);
        }

        return meshes;
    }

    void ModelImporter::ProcessNode(GameObject *parent, const aiNode* node, const aiScene* scene, const Vector3 &scale, bool flipYZ, const std::string directoryPath)
    {
        auto transformation = ToMatrix4(node->mTransformation);
        parent->GetTransform()->SetPosition(Matrix4f::ExtractTranslation(transformation) * scale);
        parent->GetTransform()->SetRotation(Matrix4f::ExtractRotation(transformation));
        parent->GetTransform()->SetScale(Matrix4f::ExtractScale(transformation));

        MeshRenderer *renderer = nullptr;
        
        if(node->mNumMeshes > 0)
        {
            std::string name(node->mName.C_Str());
            parent->SetName(name);
            renderer = parent->AddComponent<MeshRenderer>();
        }

        auto texture = Resources::FindTexture2D(Constants::GetString(ConstantString::TextureDefault));

        // Process all meshes in the current node
        for (size_t i = 0; i < node->mNumMeshes; i++) 
        {
            aiMesh* aMesh = scene->mMeshes[node->mMeshes[i]];

            std::vector<Vertex> vertices;
            vertices.resize(aMesh->mNumVertices);
            
            auto transformation = node->mTransformation * node->mParent->mTransformation;

            if(aMesh->HasTextureCoords(0))
            {
                for(size_t j = 0; j < aMesh->mNumVertices; j++)
                {
                    auto pos = aMesh->mVertices[j];
                    auto nrm = aMesh->mNormals[j];
                    auto uv = aMesh->mTextureCoords[0][j];

                    if(flipYZ)
                    {
                        float y = pos.z;
                        float z = pos.y;
                        pos.y = y;
                        pos.z = z;
                    }

                    vertices[j].position = Vector3(pos.x, pos.y, pos.z) * scale;
                    vertices[j].normal = Vector3(nrm.x, nrm.y, nrm.z);
                    vertices[j].uv = Vector2(uv.x, uv.y);
                }
            }
            else
            {
                for(size_t j = 0; j < aMesh->mNumVertices; j++)
                {
                    auto pos = aMesh->mVertices[j];
                    auto nrm = aMesh->mNormals[j];

                    if(flipYZ)
                    {
                        float y = pos.z;
                        float z = pos.y;
                        pos.y = y;
                        pos.z = z;
                    }

                    vertices[j].position = Vector3(pos.x, pos.y, pos.z) * scale;
                    vertices[j].normal = Vector3(nrm.x, nrm.y, nrm.z);
                    vertices[j].uv = Vector2(0.0f, 0.0f);
                }
            }

            std::vector<GLuint> indices;
            indices.resize(aMesh->mNumFaces * 3);
            size_t index = 0;

            for(size_t j = 0; j < aMesh->mNumFaces; j++)
            {
                indices[index+0] = aMesh->mFaces[j].mIndices[0];
                indices[index+1] = aMesh->mFaces[j].mIndices[1];
                indices[index+2] = aMesh->mFaces[j].mIndices[2];
                index += 3;
            }

            uint32_t materialIndex = aMesh->mMaterialIndex;
            aiMaterial *aMaterial = scene->mMaterials[materialIndex];

            //DumpTextureNames(aMaterial);

            auto mesh = std::make_shared<Mesh>(vertices, indices, false);
            mesh->Generate();
            mesh->SetName(aMaterial->GetName().C_Str());

            auto material = std::make_shared<DiffuseMaterial>();
            material->SetName(aMaterial->GetName().C_Str());

            aiString texturePath;

            if(aMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
            {
                auto embeddedTexture = scene->GetEmbeddedTexture(texturePath.C_Str());

                if(embeddedTexture)
                {
                    auto pTexture = LoadEmbeddedTexture(aMaterial, embeddedTexture);

                    if(pTexture != nullptr)
                        material->SetDiffuseTexture(pTexture);
                    else
                        material->SetDiffuseTexture(texture);
                }
                else
                {
                    auto tex = LoadDiffuseTexture(aMaterial, directoryPath);
                    material->SetDiffuseTexture(tex != nullptr ? tex : texture);
                }
            }
            else
            {
                material->SetDiffuseTexture(texture);
            }
            
            renderer->Add(mesh, material);
        }

        // Process all child nodes
        for (size_t i = 0; i < node->mNumChildren; i++) 
        {
            auto child = GameObject::Create();
            child->SetName(std::string(node->mChildren[i]->mName.C_Str()));
            child->GetTransform()->SetParent(parent->GetTransform());
            ProcessNode(child, node->mChildren[i], scene, scale, flipYZ, directoryPath);
        }
    }

    Texture2D *ModelImporter::LoadEmbeddedTexture(const aiMaterial *pMaterial, const aiTexture *pTexture)
    {
        unsigned int size = pTexture->mWidth;
        Image image((uint8_t*)pTexture->pcData, size);

        if(image.IsLoaded())
        {
            return Resources::AddTexture2D(pTexture->mFilename.C_Str(), Texture2D(&image));
        }

        return nullptr;
    }
}