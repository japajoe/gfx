// #ifndef GFX_GRAPHICS3D_HPP
// #define GFX_GRAPHICS3D_HPP

// #include "Vertex.hpp"
// #include "Color.hpp"
// #include <vector>
// #include <cstdint>
// #include <cstdlib>
// #include <functional>

// namespace GFX
// {
//     struct DrawListItem3D
//     {
//         uint32_t shaderId;
//         uint32_t textureId;
//         uint32_t vertexOffset;
//         uint32_t vertexCount;
//         uint32_t indiceCount;
//         uint32_t indiceOffset;
//         Vector3 position;
//         Quaternion rotation;
//         Vector3 scale;
//         void *userData;
//     };

//     struct DrawCommand3D
//     {
//         Vertex *vertices;
//         uint32_t numVertices;
//         uint32_t *indices;
//         uint32_t numIndices;
//         uint32_t textureId;
//         uint32_t shaderId;
//         Vector3 position;
//         Quaternion rotation;
//         Vector3 scale;
//         void *userData;

//         DrawCommand3D()
//         {
//             this->vertices = nullptr;
//             this->numVertices = 0;
//             this->indices = nullptr;
//             this->numIndices = 0; 
//             this->textureId = 0; 
//             this->position = Vector3(0, 0, 0);
//             this->rotation = Quaternion(1, 0, 0, 0);
//             this->scale = Vector3(1, 1, 1);
//             this->userData = nullptr;
//         }
//     };

//     struct GLStateInfo3D
//     {
//         bool depthTestEnabled;
//         bool blendEnabled;
//         int blendSrcFactor;
//         int blendDstFactor;
//         int blendEquation;
//         int depthFunc;
//     };

//     enum Uniform3D
//     {
//         Uniform_Projection,
//         Uniform_Resolution,
//         Uniform_Texture,
//         Uniform_Time,
//         Uniform_COUNT
//     };

// 	class Graphics3D
// 	{
// 	friend class Graphics;
// 	private:
//         static uint32_t VAO;
//         static uint32_t VBO;
//         static uint32_t EBO;
//         static uint32_t shaderId;
//         static uint32_t textureId;
//         static std::vector<uint32_t> uniforms;
//         static std::vector<DrawListItem3D> items;
//         static std::vector<Vertex> vertices;
//         static std::vector<uint> indices;
//         static uint32_t itemCount;
//         static uint32_t vertexCount;
//         static uint32_t indiceCount;
//         static std::vector<Vertex> vertexBufferTemp; //Temporary buffer used by some 'Add' functions with dynamic size requirements
//         static std::vector<uint> indexBufferTemp; //Temporary buffer used by some 'Add' functions with dynamic size requirements
//         static GLStateInfo3D glState;
//         static uint32_t numDrawCalls;
// 		static void Initialize();
// 		static void Deinitialize();
// 		static void NewFrame();
// 		static void CheckVertexBuffer(size_t numRequiredVertices);
// 		static void CheckIndexBuffer(size_t numRequiredIndices);
// 		static void CheckItemBuffer(size_t numRequiredItems);
// 		static void CheckTemporaryVertexBuffer(size_t numRequiredVertices);
// 		static void CheckTemporaryIndexBuffer(size_t numRequiredIndices);
// 		template <typename T>
// 		static void CheckTemporaryBuffer(std::vector<T>& buffer, int numRequiredItems)
// 		{
// 			// If the current size of the buffer is less than the required number of items
// 			if (buffer.size() < numRequiredItems) 
// 			{
// 				// Calculate new size, starting with doubling the current size
// 				int newSize = buffer.size() * 2;

// 				// Make sure the new size is large enough to hold the required items
// 				while (newSize < numRequiredItems) 
// 				{
// 					newSize *= 2;
// 				}

// 				// Resize the vector to the new size
// 				buffer.resize(newSize);
// 			}
// 		}
// 		static void AddVertices(const DrawCommand3D *command);
// 		static void StoreState();
// 		static void RestoreState();
// 		static void CreateBuffers();
// 		static void CreateShader();
// 		static void CreateTexture();
// 	public:
//         static void AddTriangles(const Vector3 *triangles, size_t count, const Vector3 &position, const Quaternion &rotation, const Vector3 &scale, const Color &color);
//         static std::string GetVertexSource();
//         static std::string GetFragmentSource();
// 	};
// }

// #endif