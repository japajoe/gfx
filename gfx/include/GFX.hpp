#ifndef GFX_GFX_HPP
#define GFX_GFX_HPP

#include "System/Numerics/Vector4.hpp"
#include "System/Numerics/Matrix4.hpp"
#include "System/Numerics/Matrix3.hpp"
#include "System/Numerics/Vector2.hpp"
#include "System/Numerics/Noise.hpp"
#include "System/Numerics/Quaternion.hpp"
#include "System/Numerics/Vector3.hpp"
#include "System/String.hpp"
#include "System/Mathf.hpp"
#include "System/BitConverter.hpp"
#include "System/Collections/ConcurrentList.hpp"
#include "System/Collections/ConcurrentQueue.hpp"
#include "System/EventHandler.hpp"
#include "System/Random.hpp"
#include "System/IO/BinaryStream.hpp"
#include "System/IO/File.hpp"
#include "External/imgui/imgui.h"
#include "External/imgui/imgui_internal.h"
#include "External/imgui/imgui_stdlib.h"
#include "External/glm/glm.hpp"
#include "Embedded/JetBrainsMonoRegular.hpp"
#include "Embedded/RobotoMonoRegular.hpp"
#include "Embedded/DejaVuSansMono.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/TextureCubeMap.hpp"
#include "Graphics/Color.hpp"
#include "Graphics/GL.hpp"
#include "Graphics/Graphics2D.hpp"
#include "Graphics/World.hpp"
#include "Graphics/Font.hpp"
#include "Graphics/Mesh.hpp"
#include "Graphics/Image.hpp"
#include "Graphics/CascadedShadowMapper.hpp"
#include "Graphics/Materials/ProceduralSkybox2Material.hpp"
#include "Graphics/Materials/DiffuseMaterial.hpp"
#include "Graphics/Materials/WaterMaterial.hpp"
#include "Graphics/Materials/DepthMaterial.hpp"
#include "Graphics/Materials/ProceduralSkyboxMaterial.hpp"
#include "Graphics/Materials/Material.hpp"
#include "Graphics/Materials/ParticleMaterial.hpp"
#include "Graphics/Materials/SkyboxMaterial.hpp"
#include "Graphics/Materials/TerrainMaterial.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Vertex.hpp"
#include "Graphics/GUITextBuffer.hpp"
#include "Graphics/Renderers/Terrain.hpp"
#include "Graphics/Renderers/MeshRenderer.hpp"
#include "Graphics/Renderers/Renderer.hpp"
#include "Graphics/Renderers/LineRenderer.hpp"
#include "Graphics/Renderers/ParticleSystem.hpp"
#include "Graphics/Renderers/PostProcessingRenderer.hpp"
#include "Graphics/Frustum.hpp"
#include "Graphics/GUILayout.hpp"
#include "Graphics/ModelImporter.hpp"
#include "Graphics/Rectangle.hpp"
#include "Graphics/Shaders/ProceduralSkybox2Shader.hpp"
#include "Graphics/Shaders/DepthShader.hpp"
#include "Graphics/Shaders/WaterShader.hpp"
#include "Graphics/Shaders/PostProcessing/HorizontalBlurShader.hpp"
#include "Graphics/Shaders/PostProcessing/GrayscaleShader.hpp"
#include "Graphics/Shaders/PostProcessing/VerticalBlurShader.hpp"
#include "Graphics/Shaders/SkyboxShader.hpp"
#include "Graphics/Shaders/CoreShaderInclude.hpp"
#include "Graphics/Shaders/TerrainShader.hpp"
#include "Graphics/Shaders/ParticleShader.hpp"
#include "Graphics/Shaders/LineShader.hpp"
#include "Graphics/Shaders/DiffuseShader.hpp"
#include "Graphics/Shaders/ProceduralSkyboxShader.hpp"
#include "Graphics/Shaders/PostProcessingShader.hpp"
#include "Graphics/Shadow.hpp"
#include "Graphics/BoundingBox.hpp"
#include "Graphics/Texture3D.hpp"
#include "Graphics/GUI.hpp"
#include "Graphics/Graphics3D.hpp"
#include "Graphics/Texture2D.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Buffers/UniformBufferObject.hpp"
#include "Graphics/Buffers/FrameBufferObject.hpp"
#include "Graphics/Buffers/VertexBufferObject.hpp"
#include "Graphics/Buffers/ElementBufferObject.hpp"
#include "Graphics/Buffers/VertexArrayObject.hpp"
#include "Graphics/Spline.hpp"
#include "Graphics/Vertex2D.hpp"
#include "Physics/RaycastHit.hpp"
#include "Physics/Joints/HingeJoint.hpp"
#include "Physics/Joints/Joint.hpp"
#include "Physics/Collision/BoxCollider.hpp"
#include "Physics/Collision/MeshCollider.hpp"
#include "Physics/Collision/SphereCollider.hpp"
#include "Physics/Collision/TerrainCollider.hpp"
#include "Physics/Collision/CapsuleCollider.hpp"
#include "Physics/Collision/Collider.hpp"
#include "Physics/Collision/WheelCollider.hpp"
#include "Physics/Collision/CylinderCollider.hpp"
#include "Physics/Rigidbody.hpp"
#include "Physics/Physics.hpp"
#include "Audio/Audio.hpp"
#include "Audio/AudioBuffer.hpp"
#include "Audio/DSP/Oscillator.hpp"
#include "Audio/DSP/AudioEffect.hpp"
#include "Audio/DSP/FMGenerator.hpp"
#include "Audio/DSP/AudioGenerator.hpp"
#include "Audio/DSP/PhaserEffect.hpp"
#include "Audio/DSP/NoiseCalculator.hpp"
#include "Audio/DSP/NoiseGenerator.hpp"
#include "Audio/DSP/Wavetable.hpp"
#include "Audio/AudioSource.hpp"
#include "Audio/AudioClip.hpp"
#include "Audio/AudioListener.hpp"
#include "Core/GameObject.hpp"
#include "Core/Debug.hpp"
#include "Core/Object.hpp"
#include "Core/Mouse.hpp"
#include "Core/GameBehaviour.hpp"
#include "Core/Camera.hpp"
#include "Core/Component.hpp"
#include "Core/AssetPack.hpp"
#include "Core/Keyboard.hpp"
#include "Core/Resources.hpp"
#include "Core/Time.hpp"
#include "Core/Application.hpp"
#include "Core/Transform.hpp"
#include "Core/Constants.hpp"
#include "Core/Light.hpp"
#include "Core/Input.hpp"
#include "Core/MouseOrbit.hpp"
#include "Core/Resource.hpp"
#include "Core/FirstPersonCamera.hpp"

using namespace GFX;

#endif