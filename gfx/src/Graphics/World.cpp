#include "World.hpp"
#include "Buffers/UniformBufferObject.hpp"
#include "../Core/Resources.hpp"
#include "../Core/Time.hpp"
#include "../Core/Constants.hpp"

namespace GFX
{
	Color World::fogColor = Color(247, 169, 90, 255);
	float World::fogDensity = 0.0005f;
	float World::fogGradient = 1.5f;
	bool World::fogEnabled = true;
	UniformBufferObject *World::ubo = nullptr;
	
	Color World::GetFogColor()
	{
		return fogColor;
	}

	void World::SetFogColor(const Color &color)
	{
		fogColor = color;
	}

	float World::GetFogDensity()
	{
		return fogDensity;
	}

	void World::SetFogDensity(float density)
	{
		fogDensity = density;
	}

	float World::GetFogGradient()
	{
		return fogGradient;
	}

	void World::SetFogGradient(float gradient)
	{
		fogGradient = gradient;
	}

	bool World::GetFogEnabled()
	{
		return fogEnabled;
	}

	void World::SetFogEnabled(bool enabled)
	{
		fogEnabled = enabled;
	}

	void World::UpdateUniformBuffer()
	{
		if(ubo == nullptr)
			ubo = Resources::FindUniformBuffer(Constants::GetString(ConstantString::UniformBufferWorld));

		if(ubo == nullptr)
			return;
	
		UniformWorldInfo info;
		info.fogColor = fogColor;
		info.fogDensity = fogDensity;
		info.fogGradient = fogGradient;
		info.fogEnabled = fogEnabled ? 1 : 0;
		info.time = Time::GetTime();

		ubo->Bind();
		ubo->BufferSubData(0, sizeof(UniformWorldInfo), &info);
		ubo->Unbind();
	}
}