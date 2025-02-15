#include "Resources.hpp"
#include "Debug.hpp"
#include "AssetPack.hpp"
#include "GameBehaviour.hpp"
#include "../System/IO/File.hpp"
#include "../Graphics/Image.hpp"
#include <future>

namespace GFX
{
	ConcurrentQueue<Resource> Resources::resourceQueue;
	ConcurrentQueue<ResourceBatch> Resources::resourceBatchQueue;
	std::unordered_map<std::string,UniformBufferObject> Resources::uniformBuffers;
	std::unordered_map<std::string,Font> Resources::fonts;
	std::unordered_map<std::string,Shader> Resources::shaders;
	std::unordered_map<std::string,Texture2D> Resources::textures2D;
	std::unordered_map<std::string,Texture3D> Resources::textures3D;
	std::unordered_map<std::string,TextureCubeMap> Resources::texturesCubemap;
	std::unordered_map<std::string,Mesh> Resources::meshes;

	static void LogAdd(const std::string &type, const std::string &name)
	{
		Debug::WriteLine("[" + type + "] " + name + " successfully added");
	}

	static void LogError(const std::string &type, const std::string &name)
	{
		Debug::WriteLine("[" + type + "] " + name + " failed to add");
	}

	UniformBufferObject *Resources::AddUniformBuffer(const std::string &name, const UniformBufferObject &ubo)
	{
		if(uniformBuffers.contains(name))
		{
			LogError("UBO", name);
			return nullptr;
		}
		uniformBuffers[name] = ubo;
		LogAdd("UBO", name);
		return &uniformBuffers[name];
	}

	UniformBufferObject *Resources::FindUniformBuffer(const std::string &name)
	{
		if(!uniformBuffers.contains(name))
			return nullptr;
		return &uniformBuffers[name];
	}

	Font *Resources::AddFont(const std::string &name, const Font &font)
	{
		if(fonts.contains(name))
		{
			LogError("FONT", name);
			return nullptr;
		}
		fonts[name] = font;
		LogAdd("FONT", name);
		return &fonts[name];
	}

	Font *Resources::FindFont(const std::string &name)
	{
		if(!fonts.contains(name))
			return nullptr;
		return &fonts[name];
	}

	Shader *Resources::AddShader(const std::string &name, const Shader &shader)
	{
		if(shaders.contains(name))
		{
			LogError("SHADER", name);
			return nullptr;
		}
		shaders[name] = shader;
		LogAdd("SHADER", name);
		return &shaders[name];
	}

	Shader *Resources::FindShader(const std::string &name)
	{
		if(!shaders.contains(name))
			return nullptr;
		return &shaders[name];
	}

	Texture2D *Resources::AddTexture2D(const std::string &name, const Texture2D &texture)
	{
		if(textures2D.contains(name))
		{
			LogError("TEXTURE2D", name);
			return nullptr;
		}
		textures2D[name] = texture;
		LogAdd("TEXTURE2D", name);
		return &textures2D[name];
	}

	Texture2D *Resources::FindTexture2D(const std::string &name)
	{
		if(!textures2D.contains(name))
			return nullptr;
		return &textures2D[name];
	}

	Texture3D *Resources::AddTexture3D(const std::string &name, const Texture3D &texture)
	{
		if(textures3D.contains(name))
		{
			LogError("TEXTURE3D", name);
			return nullptr;
		}
		textures3D[name] = texture;
		LogAdd("TEXTURE3D", name);
		return &textures3D[name];
	}

	Texture3D *Resources::FindTexture3D(const std::string &name)
	{
		if(!textures3D.contains(name))
			return nullptr;
		return &textures3D[name];
	}

	TextureCubeMap *Resources::AddTextureCubeMap(const std::string &name, const TextureCubeMap &texture)
	{
		if(texturesCubemap.contains(name))
		{
			LogError("TEXTURECUBEMAP", name);
			return nullptr;
		}
		texturesCubemap[name] = texture;
		LogAdd("TEXTURECUBEMAP", name);
		return &texturesCubemap[name];
	}
	
	TextureCubeMap *Resources::FindTextureCubeMap(const std::string &name)
	{
		if(!texturesCubemap.contains(name))
			return nullptr;
		return &texturesCubemap[name];
	}

	Mesh *Resources::AddMesh(const std::string &name, const Mesh &mesh)
	{
		if(meshes.contains(name))
		{
			LogError("MESH", name);
			return nullptr;
		}
		meshes[name] = mesh;
		LogAdd("MESH", name);
		return &meshes[name];
	}

	Mesh *Resources::FindMesh(const std::string &name)
	{
		if(!meshes.contains(name))
			return nullptr;
		return &meshes[name];
	}

	void Resources::LoadAsyncFromFile(ResourceType type, const std::string &resource)
	{
		auto result = std::async(std::launch::async, &GetFromFileAsync, type, resource);
	}

	void Resources::LoadAsyncBatchFromFile(ResourceType type, const std::vector<std::string> &resources)
	{
		//auto result = std::async(std::launch::async, &GetBatchFromFileAsync, type, resources);
		std::thread([=]() {
			GetBatchFromFileAsync(type, resources);
		}).detach();
	}

	void Resources::LoadAsyncFromAssetPack(ResourceType type, const std::string &resource, const std::string &pathToAssetPack, const std::string &assetPackKey)
	{
		auto result = std::async(std::launch::async, &GetFromPackAsync, type, resource, pathToAssetPack, assetPackKey);
	}

	void Resources::LoadAsyncBatchFromAssetPack(ResourceType type, const std::vector<std::string> &resources, const std::string &pathToAssetPack, const std::string &assetPackKey)
	{
		auto result = std::async(std::launch::async, &GetBatchFromPackAsync, type, resources, pathToAssetPack, assetPackKey);
	}

	void Resources::GetFromFileAsync(ResourceType type, const std::string &resource)
	{
		Resource info;
		info.type = type;
		info.name = resource;
		
		if(File::Exists(resource))
		{
			info.data = File::ReadAllBytes(resource);
			info.result = ResourceLoadResult::Ok;
		}
		else
		{
			info.result = ResourceLoadResult::Error;
		}

		resourceQueue.Enqueue(info);
	}

	void Resources::GetBatchFromFileAsync(ResourceType type, const std::vector<std::string> &resources)
	{
		ResourceBatch batch;
		batch.type = type;

		for(size_t i = 0; i < resources.size(); i++)
		{
			Resource info;
			info.type = type;
			info.name = resources[i];
			
			if(File::Exists(resources[i]))
			{
				if(type == ResourceType::Texture2D)
				{
					auto bytes = File::ReadAllBytes(resources[i]);
					Image image(bytes.data(), bytes.size());
					if(image.IsLoaded())
					{
						info.data.resize(image.GetDataSize());
						std::memcpy(info.data.data(), image.GetData(), image.GetDataSize());
						info.width = image.GetWidth();
						info.height = image.GetHeight();
						info.channels = image.GetChannels();
						info.result = ResourceLoadResult::Ok;
					}
					else
					{
						info.result = ResourceLoadResult::Error;
					}
				}
				else
				{
					info.data = File::ReadAllBytes(resources[i]);
					info.result = ResourceLoadResult::Ok;
				}
			}
			else
			{
				info.result = ResourceLoadResult::Error;
			}

			batch.resources.push_back(info);
		}
		
		resourceBatchQueue.Enqueue(batch);
	}

	void Resources::GetFromPackAsync(ResourceType type, const std::string &resource, const std::string &pathToAssetPack, const std::string &assetPackKey)
	{
		if(!File::Exists(pathToAssetPack))
		{
			Debug::WriteLog("The file does not exist: " + pathToAssetPack);
			return;
		}

        AssetPack pack;

		if(pack.Load(pathToAssetPack, assetPackKey))
		{
			Resource info;
			info.type = type;
			info.name = resource;
			
			if(pack.FileExists(resource))
			{
				info.data = pack.GetFileData(resource);
				info.result = ResourceLoadResult::Ok;
			}
			else
			{
				info.result = ResourceLoadResult::Error;
			}

			resourceQueue.Enqueue(info);
		}
	}

    void Resources::GetBatchFromPackAsync(ResourceType type, const std::vector<std::string> &resources, const std::string &pathToAssetPack, const std::string &assetPackKey)
    {
		if(!File::Exists(pathToAssetPack))
		{
			Debug::WriteLog("The file does not exist: " + pathToAssetPack);
			return;
		}

        AssetPack pack;

		if(pack.Load(pathToAssetPack, assetPackKey))
		{
			ResourceBatch batch;
			batch.type = type;

			for(size_t i = 0; i < resources.size(); i++)
			{
                Resource info;
                info.type = type;
                info.name = resources[i];
				
				if(pack.FileExists(resources[i]))
				{
                	info.data = pack.GetFileData(resources[i]);
					info.result = ResourceLoadResult::Ok;
				}
				else
				{
					info.result = ResourceLoadResult::Error;
				}
				
				batch.resources.push_back(info);
			}
			
			resourceBatchQueue.Enqueue(batch);
		}
		else
		{
			Debug::WriteLog("Failed to load asset pack: " + pathToAssetPack);
		}
    }

    void Resources::NewFrame()
    {
        if(resourceQueue.GetCount() > 0)
        {
            Resource resource;
            //Do only 1 asset per frame or this might block the main thread for a while
            if(resourceQueue.TryDequeue(resource))
            {
                GameBehaviour::OnBehaviourResourceLoadedAsync(resource);
            }
        }

        if(resourceBatchQueue.GetCount() > 0)
        {
            ResourceBatch batch;
            //Do only 1 batch per frame or this might block the main thread for a while
            if(resourceBatchQueue.TryDequeue(batch))
            {
                GameBehaviour::OnBehaviourResourceBatchLoadedAsync(batch);
            }
        }
    }
}