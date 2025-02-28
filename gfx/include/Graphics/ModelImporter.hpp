#ifndef GFX_MODELIMPORTER_HPP
#define GFX_MODELIMPORTER_HPP

#include "Mesh.hpp"
#include "../Core/GameObject.hpp"
#include "../System/Numerics/Vector2.hpp"
#include "../System/Numerics/Vector3.hpp"
#include "../System/Numerics/Vector4.hpp"
#include "../System/Numerics/Quaternion.hpp"
#include "../System/Numerics/Matrix3.hpp"
#include "../System/Numerics/Matrix4.hpp"
#include <memory>
#include <vector>

struct aiNode;
struct aiScene;
struct aiTexture;
struct aiMaterial;

namespace GFX
{
    enum ModelFlags_ : unsigned int
    {
        ModelFlags_CalcTangentSpace = 0x1,
        ModelFlags_JoinIdenticalVertices = 0x2,
        ModelFlags_MakeLeftHanded = 0x4,
        ModelFlags_Triangulate = 0x8,
        ModelFlags_RemoveComponent = 0x10,
        ModelFlags_GenNormals = 0x20,
        ModelFlags_GenSmoothNormals = 0x40,
        ModelFlags_SplitLargeMeshes = 0x80,
        ModelFlags_PreTransformVertices = 0x100,
        ModelFlags_LimitBoneWeights = 0x200,
        ModelFlags_ValidateDataStructure = 0x400,
        ModelFlags_ImproveCacheLocality = 0x800,
        ModelFlags_RemoveRedundantMaterials = 0x1000,
        ModelFlags_FixInfacingNormals = 0x2000,
        ModelFlags_PopulateArmatureData = 0x4000,
        ModelFlags_SortByPType = 0x8000,
        ModelFlags_FindDegenerates = 0x10000,
        ModelFlags_FindInvalidData = 0x20000,
        ModelFlags_GenUVCoords = 0x40000,
        ModelFlags_TransformUVCoords = 0x80000,
        ModelFlags_FindInstances = 0x100000,
        ModelFlags_OptimizeMeshes  = 0x200000,
        ModelFlags_OptimizeGraph  = 0x400000,
        ModelFlags_FlipUVs = 0x800000,
        ModelFlags_FlipWindingOrder  = 0x1000000,
        ModelFlags_SplitByBoneCount  = 0x2000000,
        ModelFlags_Debone  = 0x4000000,
        ModelFlags_GlobalScale = 0x8000000,
        ModelFlags_EmbedTextures  = 0x10000000,
        ModelFlags_ForceGenNormals = 0x20000000,
        ModelFlags_DropNormals = 0x40000000,
        ModelFlags_GenBoundingBoxes = 0x80000000
    };

    typedef unsigned int ModelFlags;

    class Texture2D;

    class ModelImporter
    {
    private:
        static void ProcessNode(GameObject *parent, const aiNode* node, const aiScene* scene, const Vector3 &scale, bool flipYZ, const std::string directoryPath);
        static Texture2D *LoadEmbeddedTexture(const aiMaterial *pMaterial, const aiTexture *pTexture);
    public:
        static GameObject *LoadFromFile(const std::string &filepath, ModelFlags modelFlags, const Vector3 &scale, bool flipYZ = false);
        static GameObject *LoadFromMemory(const void *memory, size_t size, ModelFlags modelFlags, const Vector3 &scale, bool flipYZ = false);
        static std::vector<std::shared_ptr<Mesh>> LoadMeshesFromFile(const std::string &filepath, ModelFlags modelFlags, const Vector3 &scale, bool flipYZ = false);
    };
}

#endif