#ifndef GFX_PARTICLESYSTEM_HPP
#define GFX_PARTICLESYSTEM_HPP

#include "Renderer.hpp"
#include "../Color.hpp"
#include "../Buffers/VertexArrayObject.hpp"
#include "../Buffers/VertexBufferObject.hpp"
#include "../Buffers/ElementBufferObject.hpp"
#include "../Materials/ParticleMaterial.hpp"
#include "../../System/Numerics/Vector2.hpp"
#include "../../System/Numerics/Vector3.hpp"
#include "../../System/Numerics/Vector4.hpp"
#include "../../System/Numerics/Quaternion.hpp"
#include "../../System/Numerics/Matrix4.hpp"
#include <memory>
#include <cstdint>

namespace GFX
{
    struct Particle
    {
        Vector3 position;
        float rotation;
        float rotationSpeed;
        float scale;
        Vector3 velocity;
        Color colorBegin;
        Color colorEnd;
        float size;
        float sizeBegin;
        float sizeEnd;
        float lifeTime;
        float lifeRemaining;
        bool active;
        Particle();
        Matrix4 GetMatrix(const Matrix4 &viewMatrix);
    };

    struct ParticleProperties
    {
        Vector3 position;
        Vector3 positionVariation;
        float rotationSpeed;
        Vector3 velocity;
        Vector3 velocityVariation;
        Color colorBegin;
        Color colorEnd;
        float sizeBegin;
        float sizeEnd;
        float sizeVariation;
        float lifeTime;
        ParticleProperties();
    };

    struct ParticleInstanceData
    {
        Matrix4 matrix;
        Color color;
        ParticleInstanceData();
    };

    enum class ParticleSpace
    {
        Local,
        World
    };

    enum class ParticleType
    {
        Capsule,
        Cube,
        Plane,
        Quad,
        Sphere
    };

	class ParticleSystem : public Renderer
	{
    private:
        ParticleSpace space;
        ParticleProperties properties;
        int32_t numParticles;
        int32_t activeParticles;
        int32_t poolIndex;
        int32_t emitAmount;
        std::vector<Mesh*> meshes;
        std::vector<Particle> particles;
        std::vector<ParticleInstanceData> particleData;
        std::shared_ptr<ParticleMaterial> material;
        VertexArrayObject VAO;
        VertexBufferObject VBO;
        VertexBufferObject instanceVBO;
        ElementBufferObject EBO;
        Mesh *pMesh;
        ParticleType particleType;
        void Initialize();
        void Update();
        void Emit(const ParticleProperties &particleProps);
	protected:
		void OnInitialize() override;
		void OnDestroy() override;
	public:
		ParticleSystem();
        void OnRender() override;
        void OnRender(Material *material, Camera *camera) override;
        void Emit(uint32_t amount);
        void Emit(uint32_t amount, const ParticleProperties &particleProps);
        uint32_t GetActiveParticles() const;
        void SetSpace(ParticleSpace space);
        ParticleSpace GetSpace() const;
        ParticleProperties *GetProperties();
        ParticleMaterial *GetMaterial() const;
	};
}

#endif