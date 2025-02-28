#include "ParticleSystem.hpp"
#include "../../Core/GameObject.hpp"
#include "../../Core/Resources.hpp"
#include "../../Core/Constants.hpp"
#include "../../Core/Time.hpp"
#include "../../System/Random.hpp"
#include "../GL.hpp"
#include "../Graphics.hpp"

namespace GFX
{
	ParticleProperties::ParticleProperties()
	{
		position = Vector3(0, 0, 0);
		positionVariation = Vector3(1, 1, 1);
		rotationSpeed = 0.0f;
		velocity = Vector3(0, 1, 0);
		velocityVariation = Vector3(0.1f, 0.1f, 0.1f);
		colorBegin = Color(0.5f,  0.5f, 0.5f, 0.5f);
		colorEnd = Color(0.1f, 0.1f, 0.1f, 0.0f);
		sizeBegin = 1.5f;
		sizeEnd = 0.1f;
		sizeVariation = 2.0f;
		lifeTime = 10.0f;
	}

	ParticleInstanceData::ParticleInstanceData()
	{
		Matrix4 scale = Matrix4f::CreateScale(1, 1, 1);
		Matrix4 translation = Matrix4f::CreateTranslation(0, 0, 0);
		Matrix4 rotation = Matrix4f::CreateRotation(0, 0, 0, 1);
		matrix = translation * rotation * scale;
		color = Color::White();
	}

	Particle::Particle()
	{
		position = Vector3f::Zero();
		rotation = 0.0f;
		rotationSpeed = 0.0f;
		scale = 1.0f;
		velocity = Vector3f::Zero();
		colorBegin = Color::Black();
		colorEnd = Color::White();
		sizeBegin = 1.0f;
		sizeEnd = 0.1f;
		size = sizeBegin;
		lifeTime = 2.0f;
		lifeRemaining = 0;
		active = false;
	}

	Matrix4 Particle::GetMatrix(const Matrix4 &viewMatrix)
	{
		Matrix4 translationMatrix = Matrix4f::CreateTranslation(position.x, position.y, position.z);

		Matrix4 rotationMatrix = Matrix4f::Identity();

		//Makes sure the particles are always facing the camera
        rotationMatrix[0][0] = viewMatrix[0][0];
        rotationMatrix[0][1] = viewMatrix[1][0];
        rotationMatrix[0][2] = viewMatrix[2][0];
        
        rotationMatrix[1][0] = viewMatrix[0][1];
        rotationMatrix[1][1] = viewMatrix[1][1];
        rotationMatrix[1][2] = viewMatrix[2][1];
        
        rotationMatrix[2][0] = viewMatrix[0][2];
        rotationMatrix[2][1] = viewMatrix[1][2];
        rotationMatrix[2][2] = viewMatrix[2][2];

		rotationMatrix = rotationMatrix * Matrix4f::CreateRotationZ(glm::radians(rotation));
		
		Matrix4 scaleMatrix = Matrix4f::CreateScale(scale, scale, scale);
		return translationMatrix * rotationMatrix * scaleMatrix;
	}


	ParticleSystem::ParticleSystem() : Renderer()
	{
		this->castShadows = false;
		this->receiveShadows = false;
		this->particleType = ParticleType::Quad;

		space = ParticleSpace::Local;
		
		const int maxParticles = 1000;
		numParticles = maxParticles;
		activeParticles = 0;
		poolIndex = numParticles - 1;

		particleData.resize(numParticles);
		particles.resize(numParticles);

		for(int i = 0; i < particleData.size(); i++)
		{
			particleData[i] = ParticleInstanceData();
			particles[i] = Particle();
		}

		emitAmount = 1;

		meshes.push_back(Resources::FindMesh(Constants::GetString(ConstantString::MeshCube)));
		meshes.push_back(Resources::FindMesh(Constants::GetString(ConstantString::MeshPlane)));
		meshes.push_back(Resources::FindMesh(Constants::GetString(ConstantString::MeshQuad)));
		meshes.push_back(Resources::FindMesh(Constants::GetString(ConstantString::MeshSphere)));

		pMesh = meshes[2];

		material = std::make_shared<ParticleMaterial>();
	}

	void ParticleSystem::OnInitialize()
	{
		Initialize();
		Graphics::Add(this);
	}

	void ParticleSystem::OnDestroy()
	{
		Graphics::Remove(this);
		EBO.Delete();
		VBO.Delete();
		VAO.Delete();
		instanceVBO.Delete();
	}

	void ParticleSystem::Initialize()
	{
		auto &vertices = pMesh->GetVertices();
		auto &indices = pMesh->GetIndices();

		if(VAO.GetId() == 0 && VBO.GetId() == 0 && instanceVBO.GetId() == 0 && EBO.GetId() == 0)
		{
			VAO.Generate();
			VBO.Generate();
			instanceVBO.Generate();
			EBO.Generate();
			
			VAO.Bind();

			VBO.Bind();
			VBO.BufferData(vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

            VAO.EnableVertexAttribArray(0);
            VAO.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, position));
            
            VAO.EnableVertexAttribArray(1);
            VAO.VertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, normal));
            
            VAO.EnableVertexAttribArray(2);
            VAO.VertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, uv));

			instanceVBO.Bind();
            instanceVBO.BufferData(particleData.size() * sizeof(ParticleInstanceData), particleData.data(), GL_STREAM_DRAW);
			
            VAO.EnableVertexAttribArray(3);
            VAO.VertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleInstanceData), (const GLvoid*)(0 * sizeof(Vector4)));
            
            VAO.EnableVertexAttribArray(4);
            VAO.VertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleInstanceData), (const GLvoid*)(1 * sizeof(Vector4)));
            
            VAO.EnableVertexAttribArray(5);
            VAO.VertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleInstanceData), (const GLvoid*)(2 * sizeof(Vector4)));
            
            VAO.EnableVertexAttribArray(6);
            VAO.VertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleInstanceData), (const GLvoid*)(3 * sizeof(Vector4)));
            
            VAO.EnableVertexAttribArray(7);
            VAO.VertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleInstanceData), (const GLvoid*)(4 * sizeof(Color)));

			VAO.VertexAttribDivisor(3, 1);
			VAO.VertexAttribDivisor(4, 1);
			VAO.VertexAttribDivisor(5, 1);
			VAO.VertexAttribDivisor(6, 1);
			VAO.VertexAttribDivisor(7, 1);

			EBO.Bind();
			EBO.BufferData(indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

			VAO.Unbind();
			VBO.Unbind();
			instanceVBO.Unbind();
			EBO.Unbind();
		}
		else
		{
            VBO.Bind();
            VBO.BufferData(vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
            EBO.Bind();
            EBO.BufferData(indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
            VBO.Unbind();
		}
	}

	static float InverseLerp(float start, float end, float value)
	{
		return (value - start) / (end - start);
	}

	void ParticleSystem::Update()
	{
		activeParticles = 0;

		Matrix4 parentMatrix = space == ParticleSpace::Local ? GetTransform()->GetModelMatrix() : Matrix4f::Identity();
		Camera *camera = Camera::GetMain();
		Matrix4 viewMatrix = camera->GetViewMatrix();

		for(int i = 0; i < particles.size(); i++)
		{
			if (particles[i].lifeRemaining <= 0.0f)
			{
				particles[i].active = false;                    
				continue;
			}

			float sizeMin = glm::min(particles[i].sizeBegin, particles[i].sizeEnd);
			float sizeMax = glm::max(particles[i].sizeBegin, particles[i].sizeEnd);
			float sizeRange = sizeMax - sizeMin;
			float timeProgress = particles[i].lifeTime - particles[i].lifeRemaining;
			float timePercentage = InverseLerp(0, particles[i].lifeTime, timeProgress);
			
			bool shrinking = particles[i].sizeEnd < particles[i].sizeBegin;
			float size = 0;

			if(shrinking)
				size = particles[i].sizeBegin - (timePercentage * sizeRange);
			else
				size = particles[i].sizeBegin + (timePercentage * sizeRange);

			if((particles[i].sizeBegin - particles[i].sizeEnd) == 0)
				size = particles[i].sizeBegin;

			Color color = Color::Lerp(particles[i].colorBegin, particles[i].colorEnd, timePercentage);

			particles[i].lifeRemaining -= Time::GetDeltaTime();
			particles[i].position += particles[i].velocity * Time::GetDeltaTime();

			float rotation = particles[i].rotationSpeed * Time::GetDeltaTime();
			particles[i].rotation += rotation;
			particles[i].scale = size;
			
			Matrix4 matrix = particles[i].GetMatrix(viewMatrix);/// * parentMatrix;

            particleData[activeParticles].matrix = matrix;
            particleData[activeParticles].color = color;
			activeParticles++;
		}

		if(activeParticles > 0)
		{
			instanceVBO.Bind();
			instanceVBO.BufferSubData(0, activeParticles * sizeof(ParticleInstanceData), particleData.data());
			instanceVBO.Unbind();
		}
	}

	void ParticleSystem::OnRender()
	{
		if (!GetGameObject()->GetIsActive())
			return;

		if(VAO.GetId() == 0)
			return;
			
		Camera *camera = Camera::GetMain();
		Transform *transform = GetTransform();

		if (!camera || !transform)
			return;

		if (!pMesh)
			return;

		if (!material)
			return;

		if (!material->GetShader())
			return;

		Update();

		if(activeParticles == 0)
			return;

		GL::DepthMask(false);
		GL::CullFace(false);
		GL::DepthTest(true);
		GL::BlendMode(true);

		material->Use(transform, camera);

		VAO.Bind();

		glDrawElementsInstanced(GL_TRIANGLES, pMesh->GetIndicesCount(), GL_UNSIGNED_INT, nullptr, activeParticles);

		VAO.Unbind();

		GL::DepthMask(true);
		GL::CullFace(true);
		GL::DepthTest(true);
		GL::BlendMode(false);
	}

	void ParticleSystem::OnRender(Material *material, Camera *camera)
	{

	}

	void ParticleSystem::Emit(const ParticleProperties &particleProps)
	{
		Particle &particle = particles[poolIndex];
		particle.active = true;            

		if(space == ParticleSpace::Local)
			particle.position = particleProps.position;
		else
			particle.position = GetTransform()->GetPosition() + particleProps.position;

		particle.position.x += particleProps.positionVariation.x * Random::Range(-1.0f, 1.0f);
		particle.position.y += particleProps.positionVariation.y * Random::Range(-1.0f, 1.0f);
		particle.position.z += particleProps.positionVariation.z * Random::Range(-1.0f, 1.0f);

		float randomRotation = 0.0f;
		particle.rotation = randomRotation;
		particle.rotationSpeed = particleProps.rotationSpeed;
		
		// Velocity
		float theta = (float)(2 * glm::pi<float>() * Random::GetNextDouble());
		float x = glm::sin(theta);
		float y = glm::cos(theta);
		float z = (glm::sin(theta) * glm::sin(theta));
		Vector3 r(x, y, z);

		particle.velocity = particleProps.velocity;
		particle.velocity.x += particleProps.velocityVariation.x * Random::Range(-1.0f, 1.0f);
		particle.velocity.y += particleProps.velocityVariation.y * Random::Range(-1.0f, 1.0f);
		particle.velocity.z += particleProps.velocityVariation.z * Random::Range(-1.0f, 1.0f);

		// Color
		particle.colorBegin = particleProps.colorBegin;
		particle.colorEnd = particleProps.colorEnd;

		particle.lifeTime = particleProps.lifeTime;
		particle.lifeRemaining = particleProps.lifeTime;
		particle.sizeBegin = particleProps.sizeBegin + particleProps.sizeVariation * (Random::Range(0.0f, 1.0f) - 0.5f);
		particle.sizeEnd = particleProps.sizeEnd;
		particle.size = particle.sizeBegin;

		poolIndex--;
		if (poolIndex < 0)
			poolIndex = particles.size() - 1;
	}

	void ParticleSystem::Emit(uint32_t amount)
	{
		if(amount > numParticles)
			amount = numParticles;

		for(uint32_t i = 0; i < amount; i++)
			Emit(properties);
	}

	void ParticleSystem::Emit(uint32_t amount, const ParticleProperties &particleProps)
	{
		if(amount > numParticles)
			amount = numParticles;

		for(uint32_t i = 0; i < amount; i++)
			Emit(particleProps);
	}

	uint32_t ParticleSystem::GetActiveParticles() const
	{
		return activeParticles;
	}

	void ParticleSystem::SetSpace(ParticleSpace space)
	{
		this->space = space;
	}

	ParticleSpace ParticleSystem::GetSpace() const
	{
		return space;
	}

	ParticleProperties *ParticleSystem::GetProperties()
	{
		return &properties;
	}

	ParticleMaterial *ParticleSystem::GetMaterial() const
	{
		return material.get();
	}
}
