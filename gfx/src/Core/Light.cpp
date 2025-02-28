#include "Light.hpp"
#include "GameObject.hpp"
#include "Resources.hpp"
#include "Constants.hpp"
#include "../Graphics/Buffers/UniformBufferObject.hpp"
#include <cstring>

namespace GFX
{
    std::vector<Light*> Light::lights;
    Light *Light::pMainLight = nullptr;
	UniformBufferObject *Light::ubo = nullptr;

    Light::Light() : Component()
    {
        type = LightType::Directional;
        color = Color(255, 255, 255, 255);
        ambient = Color(255, 255, 255, 255);
        diffuse = Color(255, 255, 255, 255);
        specular = Color(255, 255, 255, 255);
        strength = 1.0f;
        constant = 1.0f;
        linear = 0.09f;
        quadratic = 0.032f;
        cutoff = 0.0f;

        if(lights.size() == 0)
        {
            lights.resize(MAX_LIGHTS);

            for(size_t i = 0; i < lights.size(); i++)
            {
                lights[i] = nullptr;
            }
        }

        SetName("Light");
    }

    void Light::OnInitialize()
    {
		for(size_t i = 0; i < lights.size(); i++)
		{
			if(lights[i] == nullptr)
			{
				lights[i] = this;
				break;
			}
		}

        if(pMainLight == nullptr)
            pMainLight = this;
    }

    void Light::OnDestroy()
    {
		for(size_t i = 0; i < lights.size(); i++)
		{
			if(lights[i] == this)
			{
				lights[i] = nullptr;
				break;
			}
		}


        if(pMainLight == this)
        {
            pMainLight = nullptr;
        }
    }

    void Light::SetType(LightType type)
    {
        this->type = type;
    }
    
    LightType Light::GetType() const
    {
        return type;
    }

    void Light::SetColor(const Color &color)
    {
        this->color = color;
    }

    Color Light::GetColor() const
    {
        return color;
    }

    void Light::SetAmbient(const Color &ambient)
    {
        this->ambient = ambient;
    }

    Color Light::GetAmbient() const
    {
        return ambient;
    }

    void Light::SetDiffuse(const Color &diffuse)
    {
        this->diffuse = diffuse;
    }

    Color Light::GetDiffuse() const
    {
        return diffuse;
    }

    void Light::SetSpecular(const Color &specular)
    {
        this->specular = specular;
    }

    Color Light::GetSpecular() const
    {
        return specular;
    }

    void Light::SetStrength(float strength)
    {
        this->strength = strength;
    }

    float Light::GetStrength() const
    {
        return strength;
    }

    void Light::SetConstant(float constant)
    {
        this->constant = constant;
    }

    float Light::GetConstant() const
    {
        return constant;
    }

    void Light::SetLinear(float linear)
    {
        this->linear = linear;
    }

    float Light::GetLinear() const
    {
        return linear;
    }

    void Light::SetQuadratic(float quadratic)
    {
        this->quadratic = quadratic;
    }

    float Light::GetQuadratic() const
    {
        return quadratic;
    }

    void Light::SetCutoff(float cutoff)
    {
        this->cutoff = cutoff;
    }

    float Light::GetCutoff() const
    {
        return cutoff;
    }

    Light *Light::GetMain()
    {
        return pMainLight;
    }

    void Light::UpdateUniformBuffer()
    {
        if(ubo == nullptr)
            ubo = Resources::FindUniformBuffer(Constants::GetString(ConstantString::UniformBufferLights));

        if(ubo == nullptr)
            return;

        if(lights.size() == 0)
            return;

        ubo->Bind();

        for(size_t i  = 0; i < lights.size(); i++)
        {
            auto light = lights[i];
            UniformLightInfo info;

            if(light != nullptr)
            {
                info.isActive = light->GetGameObject()->GetIsActive() ? 1 : -1;
                info.type = static_cast<int>(light->GetType());
                info.constant = light->GetConstant();
                info.linear = light->GetLinear();
                info.quadratic = light->GetQuadratic();
                info.strength = light->GetStrength();
                info.cutoff = light->GetCutoff();
                info.padding1 = 0;
                info.position = Vector4(light->GetTransform()->GetPosition(), 1.0f);
                info.direction = Vector4(light->GetTransform()->GetForward(), 1.0f);
                info.color = light->GetColor();
                info.ambient = light->GetAmbient();
                info.diffuse = light->GetDiffuse();
                info.specular = light->GetSpecular();
            }
            else
            {
                info.isActive = 0;
            }

            ubo->BufferSubData(i * sizeof(UniformLightInfo), sizeof(UniformLightInfo), &info);
        }

        ubo->Unbind();
    }
}