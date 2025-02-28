#ifndef GFX_RESOURCES_HPP
#define GFX_RESOURCES_HPP

#include "../Graphics/Buffers/UniformBufferObject.hpp"
#include "../Graphics/Shader.hpp"
#include "../Graphics/Texture2D.hpp"
#include "../Graphics/Texture3D.hpp"
#include "../Graphics/TextureCubeMap.hpp"
#include "../Graphics/Font.hpp"
#include "../Graphics/Mesh.hpp"
#include "../System/Collections/ConcurrentQueue.hpp"
#include "Resource.hpp"
#include <unordered_map>
#include <vector>
#include <string>

namespace GFX
{
	class Resources
	{
	friend class Application;
	private:
		static ConcurrentQueue<Resource> resourceQueue;
		static ConcurrentQueue<ResourceBatch> resourceBatchQueue;
		static std::unordered_map<std::string,UniformBufferObject> uniformBuffers;
		static std::unordered_map<std::string,Font> fonts;
		static std::unordered_map<std::string,Shader> shaders;
		static std::unordered_map<std::string,Texture2D> textures2D;
		static std::unordered_map<std::string,Texture3D> textures3D;
		static std::unordered_map<std::string,TextureCubeMap> texturesCubemap;
		static std::unordered_map<std::string,Mesh> meshes;
		static void GetFromFileAsync(ResourceType type, const std::string &resource);
		static void GetBatchFromFileAsync(ResourceType type, const std::vector<std::string> &resources);
		static void GetFromPackAsync(ResourceType type, const std::string &resource, const std::string &pathToAssetPack, const std::string &assetPackKey);
		static void GetBatchFromPackAsync(ResourceType type, const std::vector<std::string> &resources, const std::string &pathToAssetPack, const std::string &assetPackKey);
		static void NewFrame();
	public:
		static UniformBufferObject *AddUniformBuffer(const std::string &name, const UniformBufferObject &ubo);
		static UniformBufferObject *FindUniformBuffer(const std::string &name);
		static Font *AddFont(const std::string &name, const Font &font);
		static Font *FindFont(const std::string &name);
		static Shader *AddShader(const std::string &name, const Shader &shader);
		static Shader *FindShader(const std::string &name);
		static Texture2D *AddTexture2D(const std::string &name, const Texture2D &texture);
		static Texture2D *FindTexture2D(const std::string &name);
		static Texture3D *AddTexture3D(const std::string &name, const Texture3D &texture);
		static Texture3D *FindTexture3D(const std::string &name);
		static TextureCubeMap *AddTextureCubeMap(const std::string &name, const TextureCubeMap &texture);
		static TextureCubeMap *FindTextureCubeMap(const std::string &name);
		static Mesh *AddMesh(const std::string &name, const Mesh &mesh);
		static Mesh *FindMesh(const std::string &name);
		static void LoadAsyncFromFile(ResourceType type, const std::string &resource);
		static void LoadAsyncBatchFromFile(ResourceType type, const std::vector<std::string> &resources);
		static void LoadAsyncFromAssetPack(ResourceType type, const std::string &resource, const std::string &pathToAssetPack, const std::string &assetPackKey);
		static void LoadAsyncBatchFromAssetPack(ResourceType type, const std::vector<std::string> &resources, const std::string &pathToAssetPack, const std::string &assetPackKey);
	};
}

#endif