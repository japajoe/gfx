// #include "Graphics3D.hpp"
// #include "../External/glad/glad.h"
// #include "../Core/Time.hpp"
// #include "Graphics.hpp"
// #include <cstdlib>

// namespace GFX
// {
// 	uint32_t Graphics3D::VAO = 0;
// 	uint32_t Graphics3D::VBO = 0;
// 	uint32_t Graphics3D::EBO = 0;
// 	uint32_t Graphics3D::shaderId = 0;
// 	uint32_t Graphics3D::textureId = 0;
// 	std::vector<uint32_t> Graphics3D::uniforms;
// 	std::vector<DrawListItem3D> Graphics3D::items;
// 	std::vector<Vertex> Graphics3D::vertices;
// 	std::vector<uint> Graphics3D::indices;
// 	uint32_t Graphics3D::itemCount = 0;
// 	uint32_t Graphics3D::vertexCount = 0;
// 	uint32_t Graphics3D::indiceCount = 0;
// 	std::vector<Vertex> Graphics3D::vertexBufferTemp;
// 	std::vector<uint> Graphics3D::indexBufferTemp;
// 	GLStateInfo3D Graphics3D::glState;
// 	uint32_t Graphics3D::numDrawCalls = 0;

//     void Graphics3D::Initialize() 
// 	{        
// 		VAO = 0;
// 		VBO = 0;
// 		EBO = 0;
// 		shaderId = 0;
// 		textureId = 0;
// 		uniforms.resize(Uniform_COUNT);
// 		itemCount = 0;
// 		vertexCount = 0;
// 		indiceCount = 0;
// 		numDrawCalls = 0;

// 		CreateBuffers();
// 		CreateShader();
// 		CreateTexture();
//     }

//     void Graphics3D::Deinitialize() 
// 	{
//         if(VAO > 0) 
// 		{
//             glDeleteVertexArrays(1, &VAO);
//             VAO = 0;
//         }

//         if(VBO > 0) 
// 		{
//             glDeleteBuffers(1, &VBO);
//             VBO = 0;
//         }

//         if(EBO > 0) 
// 		{
//             glDeleteBuffers(1, &EBO);
//             EBO = 0;
//         }

//         if(shaderId > 0) 
// 		{
//             glDeleteShader(shaderId);
//             shaderId = 0;
//         }

//         if(textureId > 0) 
// 		{
//             glDeleteTextures(1, &textureId);
//             textureId = 0;
//         }
//     }

// 	void Graphics3D::NewFrame()
// 	{
// 		numDrawCalls = itemCount;

// 		if(itemCount == 0) 
// 			return;

// 		auto viewport = Graphics::GetViewport();
// 		float L = viewport.x;
// 		float R = viewport.x + viewport.width;
// 		float T = viewport.y;
// 		float B = viewport.y + viewport.height;
// 		float near = -1.0f;
// 		float far = 1.0f;

//         const float projectionMatrix[4][4] = {
//             { 2.0f / (R - L),    0.0f,            0.0f,              0.0f },
//             { 0.0f,              2.0f / (T - B),  0.0f,              0.0f },
//             { 0.0f,              0.0f,           -1.0f / (far - near), 0.0f },
//             { -(R + L) / (R - L), -(T + B) / (T - B), (far + near) / (far - near), 1.0f }
//         };

// 		StoreState();

//         glDisable(GL_DEPTH_TEST);
//         glEnable(GL_BLEND);
//         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//         glBlendEquation(GL_FUNC_ADD);

//         glBindVertexArray(VAO);
//         glBindBuffer(GL_ARRAY_BUFFER, VBO);
//         glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(Vertex), vertices.data());

//         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//         glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indiceCount * sizeof(uint32_t), indices.data());

//         uint32_t lastShaderId = items[0].shaderId;
//         glUseProgram(lastShaderId);
//         glActiveTexture(GL_TEXTURE0);

//         uint32_t lastTextureId = items[0].textureId;
//         glBindTexture(GL_TEXTURE_2D, lastTextureId);

// 		size_t drawOffset = 0; // Offset for the draw call

// 		for(size_t i = 0; i < itemCount; i++) 
// 		{
// 			if(items[i].shaderId != lastShaderId) 
// 			{
//                 glUseProgram(items[i].shaderId);
//                 lastShaderId = items[i].shaderId;
// 			}

// 			if(items[i].textureId != lastTextureId) 
// 			{
//                 glBindTexture(GL_TEXTURE_2D, items[i].textureId);
//                 lastTextureId = items[i].textureId;
// 			}

// 			if(lastShaderId == shaderId) 
// 			{
// 				glUniform1i(uniforms[Uniform_Texture], 0);
// 				glUniformMatrix4fv(uniforms[Uniform_Projection], 1, GL_FALSE, &projectionMatrix[0][0]);
// 				glUniform1f(uniforms[Uniform_Time], Time::GetTime());
// 				glUniform2f(uniforms[Uniform_Resolution], viewport.width, viewport.height);
// 			}
// 			else 
// 			{
// 				// Only dispatch callback for custom shaders
// 				//These 4 uniforms are mandatory on any shader:
// 				//uTexture
// 				//uProjection
// 				//uTime
// 				//uResolution

// 				glUniform1i(glGetUniformLocation(lastShaderId, "uTexture"), 0);
// 				glUniformMatrix4fv(glGetUniformLocation(lastShaderId, "uProjection"), 1, false, &projectionMatrix[0][0]);
// 				glUniform1f(glGetUniformLocation(lastShaderId, "uTime"), Time::GetTime());
// 				glUniform2f(glGetUniformLocation(lastShaderId, "uResolution"), viewport.width, viewport.height);
				
// 			}
			
// 			glDrawElements(GL_TRIANGLES, items[i].indiceCount, GL_UNSIGNED_INT, (void*)(drawOffset * sizeof(uint32_t)));
			
// 			drawOffset += items[i].indiceCount;
// 		}

//         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//         glBindBuffer(GL_ARRAY_BUFFER, 0);
//         glBindVertexArray(0);

// 		RestoreState();

// 		// Reset counts for the next render
// 		itemCount = 0;
// 		vertexCount = 0;
// 		indiceCount = 0;
// 	}

//     void Graphics3D::AddTriangles(const Vector3 *triangles, size_t count, const Vector3 &position, const Quaternion &rotation, const Vector3 &scale, const Color &color)
//     {

//     }

// 	void Graphics3D::AddVertices(const DrawCommand3D *command) 
// 	{
// 		CheckVertexBuffer(command->numVertices);
//         CheckIndexBuffer(command->numIndices);
//         CheckItemBuffer(1);

//         memcpy(&vertices[vertexCount], &command->vertices[0], command->numVertices * sizeof(Vertex));

//         for(size_t i = 0; i < command->numIndices; i++) {
//             indices[indiceCount+i] = command->indices[i] + vertexCount;
//         }

// 		DrawListItem3D drawListItem = items[itemCount];

// 		drawListItem.vertexCount = command->numVertices;
// 		drawListItem.indiceCount = command->numIndices;
// 		drawListItem.vertexOffset = vertexCount;
// 		drawListItem.indiceOffset = indiceCount;
// 		drawListItem.shaderId = command->shaderId == 0 ? shaderId : command->shaderId;
// 		drawListItem.textureId = command->textureId;
// 		drawListItem.userData = command->userData;
		
// 		items[itemCount] = drawListItem;

// 		itemCount++;
// 		vertexCount += command->numVertices;
// 		indiceCount += command->numIndices;
// 	}

//     void Graphics3D::CheckVertexBuffer(size_t numRequiredVertices) 
// 	{
//         size_t verticesNeeded = vertexCount + numRequiredVertices;
        
//         if(verticesNeeded > vertices.size()) 
// 		{
//             size_t newSize = vertices.size() * 2;
//             while(newSize < verticesNeeded) 
// 			{
//                 newSize *= 2;
//             }
//             vertices.resize(newSize);
//             glBindBuffer(GL_ARRAY_BUFFER, VBO);
//             glBufferData(GL_ARRAY_BUFFER, newSize * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
//             glBindBuffer(GL_ARRAY_BUFFER, 0);
//         }
//     }

//     void Graphics3D::CheckIndexBuffer(size_t numRequiredIndices) 
// 	{
//         size_t indicesNeeded = indiceCount + numRequiredIndices;
        
//         if(indicesNeeded > indices.size()) 
// 		{
//             size_t newSize = indices.size() * 2;
//             while(newSize < indicesNeeded) 
// 			{
//                 newSize *= 2;
//             }
//             indices.resize(newSize);
//             glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//             glBufferData(GL_ELEMENT_ARRAY_BUFFER, newSize * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);
//             glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//         }
//     }

//     void Graphics3D::CheckItemBuffer(size_t numRequiredItems) 
// 	{
//         size_t itemsNeeded = itemCount + numRequiredItems;

//         if(itemsNeeded > items.size()) 
// 		{
//             size_t newSize = items.size() * 2;
//             while(newSize < itemsNeeded) 
// 			{
//                 newSize *= 2;
//             }
//             items.resize(newSize);
//         }
//     }

//     void Graphics3D::CheckTemporaryVertexBuffer(size_t numRequiredVertices)
// 	{
//         if(vertexBufferTemp.size() < numRequiredVertices) 
// 		{
//             size_t newSize = vertexBufferTemp.size() * 2;
//             while(newSize < numRequiredVertices) 
// 			{
//                 newSize *= 2;
//             }                
//             vertexBufferTemp.resize(newSize);
//         }
//     }

//     void Graphics3D::CheckTemporaryIndexBuffer(size_t numRequiredIndices)
// 	{
//         if(indexBufferTemp.size() < (numRequiredIndices)) 
// 		{
//             size_t newSize = indexBufferTemp.size() * 2;
//             while(newSize < numRequiredIndices) 
// 			{
//                 newSize *= 2;
//             }
//             indexBufferTemp.resize(newSize);
//         }
//     }

//     void Graphics3D::StoreState() 
// 	{
//         glState.depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
//         glState.blendEnabled = glIsEnabled(GL_BLEND);
//         glGetIntegerv(GL_BLEND_SRC, &glState.blendSrcFactor);
//         glGetIntegerv(GL_BLEND_DST, &glState.blendDstFactor);
//         glGetIntegerv(GL_BLEND_EQUATION, &glState.blendEquation);
//         glGetIntegerv(GL_DEPTH_FUNC, &glState.depthFunc);
//     }

//     void Graphics3D::RestoreState() 
// 	{
//         if (glState.depthTestEnabled)
//             glEnable(GL_DEPTH_TEST);
//         else
//             glDisable(GL_DEPTH_TEST);

//         if (glState.blendEnabled)
//             glEnable(GL_BLEND);
//         else
//             glDisable(GL_BLEND);

//         glBlendFunc(glState.blendSrcFactor, glState.blendDstFactor);
//         glBlendEquation(glState.blendEquation);
//         glDepthFunc(glState.depthFunc);
//     }

//     void Graphics3D::CreateBuffers()
// 	{
// 		constexpr size_t sizeItems = 1024;
// 		constexpr size_t sizeVertices = 2 << 15;
// 		constexpr size_t sizeIndices = 2 << 15;
// 		constexpr size_t sizeTextColors = 128;

//         items.resize(sizeItems);
//         vertices.resize(sizeVertices);
//         indices.resize(sizeIndices);
//         vertexBufferTemp.resize(sizeVertices);
//         indexBufferTemp.resize(sizeIndices);

//         glGenVertexArrays(1, &VAO);
//         glGenBuffers(1, &VBO);
//         glGenBuffers(1, &EBO);

//         glBindVertexArray(VAO);

//         glBindBuffer(GL_ARRAY_BUFFER, VBO);

//         glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

//         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
//         glEnableVertexAttribArray(0);
//         glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
//         glEnableVertexAttribArray(1);
//         glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, uv));
//         glEnableVertexAttribArray(2);

//         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        
//         glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);

//         glBindVertexArray(0);
//     }

//     static bool CheckShader(uint32_t handle, const char* desc) 
// 	{
//         GLint status = 0, log_length = 0;
//         glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
//         glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &log_length);
//         if ((GLboolean)status == GL_FALSE)
//             fprintf(stderr, "ERROR: failed to compile %s!\n", desc);
//         if (log_length > 1) 
// 		{
//             std::vector<char> buf;
//             buf.resize((int)(log_length + 1));
//             glGetShaderInfoLog(handle, log_length, nullptr, &buf[0]);
//             fprintf(stderr, "%s\n", buf.begin());
//         }

//         return (GLboolean)status == GL_TRUE;
//     }

//     static bool CheckProgram(GLuint handle, const char* desc) 
// 	{
//         GLint status = 0, log_length = 0;
//         glGetProgramiv(handle, GL_LINK_STATUS, &status);
//         glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &log_length);
//         if ((GLboolean)status == GL_FALSE)
//             fprintf(stderr, "ERROR: failed to link %s!n", desc);
//         if (log_length > 1) 
// 		{
//             std::vector<char> buf;
//             buf.resize((int)(log_length + 1));
//             glGetProgramInfoLog(handle, log_length, nullptr, &buf[0]);
//             fprintf(stderr, "%s\n", buf.begin());
//         }
//         return (GLboolean)status == GL_TRUE;
//     }

//     void Graphics3D::CreateShader()
// 	{
//         std::string vertexSource = GetVertexSource();
//         std::string fragmentSource = GetFragmentSource();

//         const GLchar* vertex_shader[1] = {
//             vertexSource.c_str()
//         };

//         const GLchar* fragment_shader[1] = {
//             fragmentSource.c_str()
//         };

//         GLuint vert_handle = glCreateShader(GL_VERTEX_SHADER);
//         glShaderSource(vert_handle, 1, vertex_shader, nullptr);
//         glCompileShader(vert_handle);
//         CheckShader(vert_handle, "vertex shader");

//         GLuint frag_handle = glCreateShader(GL_FRAGMENT_SHADER);
//         glShaderSource(frag_handle, 1, fragment_shader, nullptr);
//         glCompileShader(frag_handle);
//         CheckShader(frag_handle, "fragment shader");

//         shaderId = glCreateProgram();
//         glAttachShader(shaderId, vert_handle);
//         glAttachShader(shaderId, frag_handle);
//         glLinkProgram(shaderId);
//         CheckProgram(shaderId, "shader program");

//         glDetachShader(shaderId, vert_handle);
//         glDetachShader(shaderId, frag_handle);
//         glDeleteShader(vert_handle);
//         glDeleteShader(frag_handle);

// 		uniforms[Uniform_Texture] = glGetUniformLocation(shaderId, "uTexture");
// 		uniforms[Uniform_Resolution] = glGetUniformLocation(shaderId, "uResolution");
// 		uniforms[Uniform_Projection] = glGetUniformLocation(shaderId, "uProjection");
// 		uniforms[Uniform_Time] = glGetUniformLocation(shaderId, "uTime");
//     }

//     void Graphics3D::CreateTexture()
// 	{
//         unsigned char textureData[16];
//         memset(textureData, 255, 16);

//         glGenTextures(1, &textureId);
//         glBindTexture(GL_TEXTURE_2D, textureId);

//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
//     }

//     std::string Graphics3D::GetVertexSource()
//     {
//         std::string vertexSource = R"(#version 330 core
// layout(location = 0) in vec2 aPosition;
// layout(location = 1) in vec2 aTexCoord;
// layout(location = 2) in vec4 aColor;

// uniform mat4 uProjection;
// out vec2 oTexCoord;
// out vec4 oColor;

// void main() {
//     gl_Position = uProjection * vec4(aPosition.x, aPosition.y, 0.0, 1.0);
//     oTexCoord = aTexCoord;
//     oColor = aColor;
// })";
//         return vertexSource;
//     }

//     std::string Graphics3D::GetFragmentSource()
//     {
//         std::string fragmentSource = R"(#version 330 core
// uniform sampler2D uTexture;
// uniform float uTime;
// uniform vec2 uResolution;
// uniform int uIsFont;
// uniform int uFontHasSDF;

// in vec2 oTexCoord;
// in vec4 oColor;
// out vec4 FragColor;

// void main() {
//     if(uIsFont > 0) {
//         if(uFontHasSDF > 0) {
//             vec4 sample = texture(uTexture, oTexCoord);
//             float d = sample.r;
//             float aaf = fwidth(d);
//             float alpha = smoothstep(0.5 - aaf, 0.5 + aaf, d);
//             FragColor = vec4(oColor.rgb, alpha) * oColor;
//         } else {
//             vec4 sample = texture(uTexture, oTexCoord);

//             if(sample.r == 0.0)
//                 discard;

//             FragColor = vec4(oColor.rgb, 1.0) * sample.r;
//         }
//     } else {
//         FragColor = texture(uTexture, oTexCoord) * oColor;
//     }
// })";
//         return fragmentSource;
//     }
// }