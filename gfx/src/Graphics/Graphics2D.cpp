#include "Graphics2D.hpp"
#include "../External/glad/glad.h"
#include "Graphics.hpp"
#include "../Core/Time.hpp"
#include <cstdlib>

namespace GFX
{
	uint32_t Graphics2D::VAO = 0;
	uint32_t Graphics2D::VBO = 0;
	uint32_t Graphics2D::EBO = 0;
	uint32_t Graphics2D::shaderId = 0;
	uint32_t Graphics2D::textureId = 0;
	std::vector<uint32_t> Graphics2D::uniforms;
	std::vector<DrawListItem> Graphics2D::items;
	std::vector<Vertex2D> Graphics2D::vertices;
	std::vector<uint32_t> Graphics2D::indices;
	uint32_t Graphics2D::itemCount = 0;
	uint32_t Graphics2D::vertexCount = 0;
	uint32_t Graphics2D::indiceCount = 0;
	std::vector<Vertex2D> Graphics2D::vertexBufferTemp;
	std::vector<uint32_t> Graphics2D::indexBufferTemp;
	std::vector<TextColorInfo> Graphics2D::textColorInfoTemp;
	GLStateInfo Graphics2D::glState;
	uint32_t Graphics2D::numDrawCalls = 0;
	UniformUpdateCallback Graphics2D::onUniformUpdate = nullptr;

    void Graphics2D::Initialize() 
	{        
		VAO = 0;
		VBO = 0;
		EBO = 0;
		shaderId = 0;
		textureId = 0;
		uniforms.resize(Uniform_COUNT);
		itemCount = 0;
		vertexCount = 0;
		indiceCount = 0;
		numDrawCalls = 0;
		onUniformUpdate = nullptr;

		CreateBuffers();
		CreateShader();
		CreateTexture();
    }

    void Graphics2D::Deinitialize() 
	{
        if(VAO > 0) 
		{
            glDeleteVertexArrays(1, &VAO);
            VAO = 0;
        }

        if(VBO > 0) 
		{
            glDeleteBuffers(1, &VBO);
            VBO = 0;
        }

        if(EBO > 0) 
		{
            glDeleteBuffers(1, &EBO);
            EBO = 0;
        }

        if(shaderId > 0) 
		{
            glDeleteShader(shaderId);
            shaderId = 0;
        }

        if(textureId > 0) 
		{
            glDeleteTextures(1, &textureId);
            textureId = 0;
        }
    }

	void Graphics2D::NewFrame()
	{
		numDrawCalls = itemCount;

		if(itemCount == 0) 
			return;

		auto viewport = Graphics::GetViewport();
		float L = viewport.x;
		float R = viewport.x + viewport.width;
		float T = viewport.y;
		float B = viewport.y + viewport.height;
		float near = -1.0f;
		float far = 1.0f;

        const float projectionMatrix[4][4] = {
            { 2.0f / (R - L),    0.0f,            0.0f,              0.0f },
            { 0.0f,              2.0f / (T - B),  0.0f,              0.0f },
            { 0.0f,              0.0f,           -1.0f / (far - near), 0.0f },
            { -(R + L) / (R - L), -(T + B) / (T - B), (far + near) / (far - near), 1.0f }
        };

		StoreState();

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquation(GL_FUNC_ADD);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(Vertex2D), vertices.data());

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indiceCount * sizeof(uint32_t), indices.data());

        uint32_t lastShaderId = items[0].shaderId;
        glUseProgram(lastShaderId);
        glActiveTexture(GL_TEXTURE0);

        uint32_t lastTextureId = items[0].textureId;
        glBindTexture(GL_TEXTURE_2D, lastTextureId);

		size_t drawOffset = 0; // Offset for the draw call

		for(size_t i = 0; i < itemCount; i++) 
		{
			Rectangle rect = items[i].clippingRect;
			bool scissorEnabled = false;

			if(!rect.IsZero()) 
			{
                glEnable(GL_SCISSOR_TEST);
                glScissor(rect.x, rect.y, rect.width, rect.height);
                scissorEnabled = true;
			}

			if(items[i].shaderId != lastShaderId) 
			{
                glUseProgram(items[i].shaderId);
                lastShaderId = items[i].shaderId;
			}

			if(items[i].textureId != lastTextureId) 
			{
                glBindTexture(GL_TEXTURE_2D, items[i].textureId);
                lastTextureId = items[i].textureId;
			}

			if(lastShaderId == shaderId) 
			{
				glUniform1i(uniforms[Uniform_Texture], 0);
				glUniformMatrix4fv(uniforms[Uniform_Projection], 1, GL_FALSE, &projectionMatrix[0][0]);
				glUniform1f(uniforms[Uniform_Time], Time::GetTime());
				glUniform2f(uniforms[Uniform_Resolution], viewport.width, viewport.height);
				// //These uniforms are only mandatory on default shader
				glUniform1i(uniforms[Uniform_IsFont], items[i].textureIsFont ? 1 : 0);
				glUniform1i(uniforms[Uniform_FontHasSDF], items[i].fontHasSDF ? 1 : 0);
			}
			else 
			{
				// Only dispatch callback for custom shaders
				//These 4 uniforms are mandatory on any shader:
				//uTexture
				//uProjection
				//uTime
				//uResolution

				glUniform1i(glGetUniformLocation(lastShaderId, "uTexture"), 0);
				glUniformMatrix4fv(glGetUniformLocation(lastShaderId, "uProjection"), 1, false, &projectionMatrix[0][0]);
				glUniform1f(glGetUniformLocation(lastShaderId, "uTime"), Time::GetTime());
				glUniform2f(glGetUniformLocation(lastShaderId, "uResolution"), viewport.width, viewport.height);
				
				if(onUniformUpdate)
					onUniformUpdate(lastShaderId, items[i].userData);
			}

			if(items[i].textureIsFont)
				glDepthMask(false);
			
			glDrawElements(GL_TRIANGLES, items[i].indiceCount, GL_UNSIGNED_INT, (void*)(drawOffset * sizeof(uint32_t)));
			
			if(items[i].textureIsFont)
				glDepthMask(true);

			drawOffset += items[i].indiceCount;

			if(scissorEnabled) 
			{
				glDisable(GL_SCISSOR_TEST);
			}
		}

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

		RestoreState();

		glDisable(GL_SCISSOR_TEST);

		// Reset counts for the next render
		itemCount = 0;
		vertexCount = 0;
		indiceCount = 0;
	}

	void Graphics2D::AddRectangle(const Vector2 &position, const Vector2 &size, float rotationDegrees, const Color &color, const Rectangle clippingRect, uint32_t shaderId, void *userData)
	{
        Vertex2D vertices[4] = {
            { Vector2(position.x, position.y), Vector2(0, 1), color }, // top left
            { Vector2(position.x, position.y + size.y), Vector2(0, 0), color }, // bottom left
            { Vector2(position.x + size.x, position.y + size.y), Vector2(1, 0), color }, // bottom right
            { Vector2(position.x + size.x, position.y), Vector2(1, 1), color }  // top right
        };

        if(rotationDegrees != 0.0f)
            RotateVertices(vertices, 4, rotationDegrees);

        uint32_t indices[6] = {
            0, 1, 2, 
            0, 2, 3
        };

        DrawCommand command;
		command.vertices = vertices;
		command.indices = indices;
		command.numVertices = 4;
		command.numIndices = 6;
		command.textureId = textureId;
		command.textureIsFont = false;
		command.fontHasSDF = false;
		command.shaderId = shaderId;
		command.clippingRect = clippingRect;
		command.userData = userData;

        AddVertices(&command);
	}

	void Graphics2D::AddRectangleRounded(const Vector2 &position, const Vector2 &size, float rotationDegrees, float radius, const Color &color, const Rectangle clippingRect, uint32_t shaderId, void *userData)
	{
		AddRectangleRoundedEx(position, size, rotationDegrees, radius, 0.0f, 0.0f, 0.0f, 0.0f, color, clippingRect, shaderId, userData);
	}

	void Graphics2D::AddRectangleRoundedEx(const Vector2 &position, const Vector2 &size, float rotationDegrees, float radius, float topLeftRadius, float topRightRadius, float bottomLeftRadius, float bottomRightRadius, const Color &color, const Rectangle clippingRect, uint32_t shaderId, void *userData)
	{
		//Source https://github.com/bburrough/RoundedQuadMesh
		float roundEdges = 1.0f * radius;
		float roundBottomLeft = bottomLeftRadius;
		float roundBottomRight = bottomRightRadius;
		float roundTopLeft = topLeftRadius;
		float roundTopRight = topRightRadius;
		bool usePercentage = false;
		Rectangle rect(position.x, position.y, size.x, size.y);
		size_t cornerVertexCount = 8;

		size_t requiredVertices = (cornerVertexCount * 4) + 1;
		size_t requiredIndices = (cornerVertexCount * 4) * 3;

		CheckTemporaryVertexBuffer(requiredVertices);
		CheckTemporaryIndexBuffer(requiredIndices);

		size_t count = cornerVertexCount * 4;

		float bl = glm::max(0.0f, roundBottomLeft + roundEdges);
		float br = glm::max(0.0f, roundBottomRight + roundEdges);
		float tl = glm::max(0.0f, roundTopLeft + roundEdges);
		float tr = glm::max(0.0f, roundTopRight + roundEdges);
		float f = (float)(glm::pi<float>() * 0.5f / (cornerVertexCount - 1));
		float a1 = 1.0f;
		float a2 = 1.0f;
		float x = 1.0f;
		float y = 1.0f;
		Vector2 rs(1, 1);

		if (usePercentage)
		{
			rs = Vector2(rect.width, rect.height) * 0.5f;
			if (rect.width > rect.height)
				a1 = rect.height / rect.width;
			else
				a2 = rect.width / rect.height;
			bl = glm::clamp(bl, 0.0f, 1.0f);
			br = glm::clamp(br, 0.0f, 1.0f);
			tl = glm::clamp(tl, 0.0f, 1.0f);
			tr = glm::clamp(tr, 0.0f, 1.0f);
		}
		else
		{
			x = rect.width * 0.5f;
			y = rect.height * 0.5f;
			if (bl + br > rect.width)
			{
				float b = rect.width / (bl + br);
				bl *= b;
				br *= b;
			}
			if (tl + tr > rect.width)
			{
				float b = rect.width / (tl + tr);
				tl *= b;
				tr *= b;
			}
			if (bl + tl > rect.height)
			{
				float b = rect.height / (bl + tl);
				bl *= b;
				tl *= b;
			}
			if (br + tr > rect.height)
			{
				float b = rect.height / (br + tr);
				br *= b;
				tr *= b;
			}
		}

		Vector2 rectCenter = rect.GetCenter();
		Vertex2D v = vertexBufferTemp[0];
		v.position = rectCenter;
		v.uv = Vector2(1, 1) * 0.5f;
		v.color = color;
		vertexBufferTemp[0] = v;

		for(size_t i = 0; i < cornerVertexCount; i++ )
		{
			float s = (float)glm::sin((float)i * f);
			float c = (float)glm::cos((float)i * f);
			Vector2 v1(-x + (1.0f - c) * bl * a1, y - (1.0f - s) * bl * a2);
			Vector2 v2(x - (1.0f - s) * br * a1, y - (1.0f - c) * br * a2);
			Vector2 v3(x - (1.0f - c) * tr * a1, -y + (1.0f - s) * tr * a2);
			Vector2 v4(-x + (1.0f - s) * tl * a1, -y + (1.0f - c) * tl * a2);

			Vertex2D vert1 = vertexBufferTemp[1 + i];
			Vertex2D vert2 = vertexBufferTemp[1 + cornerVertexCount + i];
			Vertex2D vert3 = vertexBufferTemp[1 + cornerVertexCount * 2 + i];
			Vertex2D vert4 = vertexBufferTemp[1 + cornerVertexCount * 3 + i];

			vert1.position = ((v1 * rs) + rectCenter);
			vert2.position = ((v2 * rs) + rectCenter);
			vert3.position = ((v3 * rs) + rectCenter);
			vert4.position = ((v4 * rs) + rectCenter);
			
			if(!usePercentage)
			{
				Vector2 adj(2.0f / rect.width, 2.0f / rect.height);
				v1 = (v1 * adj);
				v2 = (v2 * adj);
				v3 = (v3 * adj);
				v4 = (v4 * adj);
			}
			
			vert1.uv = v1 * 0.5f + Vector2(1, 1) * 0.5f;
			vert2.uv = v2 * 0.5f + Vector2(1, 1) * 0.5f;
			vert3.uv = v3 * 0.5f + Vector2(1, 1) * 0.5f;
			vert4.uv = v4 * 0.5f + Vector2(1, 1) * 0.5f;

			vert1.color = color;
			vert2.color = color;
			vert3.color = color;
			vert4.color = color;

			vertexBufferTemp[1 + i] = vert1;
			vertexBufferTemp[1 + cornerVertexCount + i] = vert2;
			vertexBufferTemp[1 + cornerVertexCount * 2 + i] = vert3;
			vertexBufferTemp[1 + cornerVertexCount * 3 + i] = vert4;
		}

		for(size_t i = 0; i < count; i++)
		{
			indexBufferTemp[i*3 + 0] = 0;
			indexBufferTemp[i*3 + 1] = (uint32_t)(i + 1);
			indexBufferTemp[i*3 + 2] = (uint32_t)(i + 2);
		}
		
		indexBufferTemp[count * 3 - 1] = 1;

		if(rotationDegrees != 0.0f)
			RotateVertices(vertexBufferTemp.data(), requiredVertices, rotationDegrees);

		DrawCommand command;
		command.vertices = vertexBufferTemp.data();
		command.indices = indexBufferTemp.data();
		command.numVertices = requiredVertices;
		command.numIndices = requiredIndices;
		command.textureId = textureId;
		command.textureIsFont = false;
		command.fontHasSDF = false;
		command.shaderId = shaderId;
		command.clippingRect = clippingRect;
		command.userData = userData;

		AddVertices(&command);
	}

	void Graphics2D::AddCircle(const Vector2 &position, float radius, int segments, float rotationDegrees, const Color &color, const Rectangle clippingRect, uint32_t shaderId, void *userData)
	{
        if(segments < 3)
            segments = 3;

        size_t requiredVertices = segments;
        size_t requiredIndices = segments * 3; // 3 indices per segment

        CheckTemporaryVertexBuffer(requiredVertices);
        CheckTemporaryIndexBuffer(requiredIndices);

        for (int i = 0; i < segments; ++i) 
		{
            float angle = 2.0f * glm::pi<float>() * i / segments;
            vertexBufferTemp[i].position = Vector2(radius * cos(angle), radius * sin(angle));
            vertexBufferTemp[i].uv = Vector2(0.5f + 0.5f * cos(angle), 0.5f + 0.5f * sin(angle));
            vertexBufferTemp[i].color = color;
            vertexBufferTemp[i].position.x += position.x;
            vertexBufferTemp[i].position.y += position.y;
        }

        if(rotationDegrees != 0.0f)
            RotateVertices(vertexBufferTemp.data(), segments, rotationDegrees);

        for (int i = 0; i < segments; ++i) 
		{
            indexBufferTemp[i * 3] = 0; // Center vertex (if added at 0 index)
            indexBufferTemp[i * 3 + 1] = (i + 1) % segments; // Wrap around to form a circle
            indexBufferTemp[i * 3 + 2] = i;
        }

        DrawCommand command;
        command.vertices = vertexBufferTemp.data();
        command.indices = indexBufferTemp.data();
		command.numVertices = segments;
		command.numIndices = segments * 3;
		command.textureId = textureId;
		command.textureIsFont = false;
		command.fontHasSDF = false;
		command.shaderId = shaderId;
		command.clippingRect = clippingRect;
		command.userData = userData;

        AddVertices(&command);
	}

	void Graphics2D::AddTriangle(const Vector2 &position, const Vector2 &size, float rotationDegrees, const Color &color, const Rectangle clippingRect, uint32_t shaderId, void *userData)
	{
        float halfWidth = 1.0f * 0.5f;
        float halfHeight = 1.0f * 0.5f;

        const float radius = size.x / sqrt(3); // Calculate the radius

        Vector2 vertex1(position.x, position.y + radius); // Top vertex
        Vector2 vertex2(position.x - radius * glm::sin(glm::pi<float>() / 3), position.y - radius * glm::cos(glm::pi<float>() / 3)); // Bottom-left vertex
        Vector2 vertex3(position.x + radius * glm::sin(glm::pi<float>() / 3), position.y - radius * glm::cos(glm::pi<float>() / 3)); // Bottom-right vertex

        Vector2 uv1(0.5f, 1.0f); // Top vertex UV
        Vector2 uv2(0.0f, 0.0f); // Bottom-left vertex UV
        Vector2 uv3(1.0f, 0.0f); // Bottom-right vertex UV

        Vertex2D vertices[3] = {
            { vertex1, uv1, color},
            { vertex2, uv2, color},
            { vertex3, uv3, color}
        };

        if(rotationDegrees != 0.0f)
            RotateVertices(vertices, 3, rotationDegrees);

        uint32_t indices[3] = {
            0, 2, 1, 
        };

        DrawCommand command;
        command.vertices = vertices;
        command.indices = indices;
		command.numVertices = 3;
		command.numIndices = 3;
		command.textureId = textureId;
		command.textureIsFont = false;
		command.fontHasSDF = false;
		command.shaderId = shaderId;
		command.clippingRect = clippingRect;
		command.userData = userData;

        AddVertices(&command);
	}

	void Graphics2D::AddBorder(const Vector2 &position, const Vector2 &size, float thickness, BorderOptions borderOptions, const Color &color, const Rectangle clippingRect, uint32_t shaderId, void *userData)
	{
        Rectangle outerRect(position.x, position.y, size.x, size.y);
        float innerOffset = 0.0f;

        Vector2 innerTopLeft(outerRect.x + innerOffset, outerRect.y + innerOffset);
        Vector2 innerTopRight(outerRect.x + outerRect.width - innerOffset, outerRect.y + innerOffset);
        Vector2 innerBottomLeft(outerRect.x + innerOffset, outerRect.y + outerRect.height - innerOffset);
        Vector2 innerBottomRight(outerRect.x + outerRect.width - innerOffset, outerRect.y + outerRect.height - innerOffset);

        // Fixed size for maximum lines (up to 8 for 4 borders)
        Vector2 lines[8];
        int lineCount = 0;

        // Check each border option and add the corresponding lines
        if (borderOptions & BorderOptions_Top) 
		{
            lines[lineCount++] = innerTopLeft;
            lines[lineCount++] = innerTopRight;
        }
        if (borderOptions & BorderOptions_Right) 
		{
            lines[lineCount++] = innerTopRight;
            lines[lineCount++] = innerBottomRight;
        }
        if (borderOptions & BorderOptions_Bottom) 
		{
            lines[lineCount++] = innerBottomRight;
            lines[lineCount++] = innerBottomLeft;
        }
        if (borderOptions & BorderOptions_Left) 
		{
            lines[lineCount++] = innerBottomLeft;
            lines[lineCount++] = innerTopLeft;
        }

        AddLines(lines, lineCount / 2, thickness, color, clippingRect);
	}

	void Graphics2D::AddLine(const Vector2 &p1, Vector2 p2, float thickness, const Color &color, const Rectangle clippingRect, uint32_t shaderId, void *userData)
	{
        Vector2 direction(p2.x - p1.x, p2.y - p1.y);
        float length = glm::sqrt(direction.x * direction.x + direction.y * direction.y);

        if (length == 0) 
			return;

        direction.x /= length;
        direction.y /= length;

        Vector2 perpendicular = Vector2(-direction.y * thickness * 0.5f, 
                                        direction.x * thickness * 0.5f);

        Vertex2D vertices[4] = {
            { Vector2(p1.x + perpendicular.x, p1.y + perpendicular.y), Vector2(0, 1), color }, // Bottom left
            { Vector2(p1.x - perpendicular.x, p1.y - perpendicular.y), Vector2(0, 0), color }, // Top left
            { Vector2(p2.x - perpendicular.x, p2.y - perpendicular.y), Vector2(1, 0), color }, // Top right
            { Vector2(p2.x + perpendicular.x, p2.y + perpendicular.y), Vector2(1, 1), color }  // Bottom right
        };

        uint32_t indices[6] = {
            0, 2, 1, 
            0, 3, 2
        };

        DrawCommand command;
        command.vertices = vertices;
        command.indices = indices;
		command.numVertices = 4;
		command.numIndices = 6;
		command.textureId = textureId;
		command.textureIsFont = false;
		command.fontHasSDF = false;
		command.shaderId = shaderId;
		command.clippingRect = clippingRect;
		command.userData = userData;


        AddVertices(&command);
	}

	void Graphics2D::AddLines(const Vector2 *segments, int count, float thickness, const Color &color, const Rectangle clippingRect, uint32_t shaderId, void *userData)
	{
        if (count == 0) 
            return;

        if (segments == nullptr) 
            return;

        size_t requiredVertices = count * 4; // 4 vertices per line
        size_t requiredIndices = count * 6; // 6 indices per line

        CheckTemporaryVertexBuffer(requiredVertices);
        CheckTemporaryIndexBuffer(requiredIndices);

        size_t pointCount = count * 2;
        size_t vertexIndex = 0;
        size_t indiceIndex = 0;

        for(size_t i = 0; i < pointCount; i+=2) 
		{
            Vector2 p1 = segments[i+0];
            Vector2 p2 = segments[i+1];

            Vector2 direction(p2.x - p1.x, p2.y - p1.y);

            float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

            if (length == 0) 
                return;

            direction.x /= length;
            direction.y /= length;

            Vector2 perpendicular = Vector2(-direction.y * thickness * 0.5f, 
                                            direction.x * thickness * 0.5f);

            vertexBufferTemp[vertexIndex+0] = { Vector2(p1.x + perpendicular.x, p1.y + perpendicular.y), Vector2(0, 1), color };
            vertexBufferTemp[vertexIndex+1] = { Vector2(p1.x - perpendicular.x, p1.y - perpendicular.y), Vector2(0, 0), color };
            vertexBufferTemp[vertexIndex+2] = { Vector2(p2.x - perpendicular.x, p2.y - perpendicular.y), Vector2(1, 0), color };
            vertexBufferTemp[vertexIndex+3] = { Vector2(p2.x + perpendicular.x, p2.y + perpendicular.y), Vector2(1, 1), color };

            indexBufferTemp[indiceIndex+0] = 0 + vertexIndex;
            indexBufferTemp[indiceIndex+1] = 2 + vertexIndex;
            indexBufferTemp[indiceIndex+2] = 1 + vertexIndex;
            indexBufferTemp[indiceIndex+3] = 0 + vertexIndex;
            indexBufferTemp[indiceIndex+4] = 3 + vertexIndex;
            indexBufferTemp[indiceIndex+5] = 2 + vertexIndex;

            vertexIndex += 4;
            indiceIndex += 6;
        }

        DrawCommand command;
        command.vertices = vertexBufferTemp.data();
        command.indices = indexBufferTemp.data();
		command.numVertices = vertexIndex;
		command.numIndices = indiceIndex;
		command.textureId = textureId;
		command.textureIsFont = false;
		command.fontHasSDF = false;
		command.shaderId = shaderId;
		command.clippingRect = clippingRect;
		command.userData = userData;

        AddVertices(&command);
	}

	void Graphics2D::AddPlotLines(const Vector2 &position, const Vector2 &size, const float *data, int valuesCount, float thickness, const Color &color, float scaleMin, float scaleMax, const Rectangle clippingRect, uint32_t shaderId, void *userData)
	{
        if (valuesCount < 2) 
            return;

        if (data == nullptr) 
            return;

        size_t count = valuesCount - 1;
        size_t requiredVertices = count * 4; // 4 vertices per line
        size_t requiredIndices = count * 6; // 6 indices per line
        float plotWidth = size.x;
        float plotHeight = size.y;
        float step = plotWidth / valuesCount;

        if (scaleMin == FLT_MAX || scaleMax == FLT_MAX) 
		{
            float minValue = FLT_MAX;
            float maxValue = -FLT_MAX;

            for(size_t i = 0; i < valuesCount; i++) 
			{
                const float v = data[i];
                if (v != v) // Ignore NaN values
                    continue;
                minValue = glm::min(minValue, v);
                maxValue = glm::max(maxValue, v);
            }

            if (scaleMin == FLT_MAX)
                scaleMin = minValue;
            if (scaleMax == FLT_MAX)
                scaleMax = maxValue;
        }

        auto normalize = [] (float x, float scaleMin, float scaleMax) {
            if (scaleMax == scaleMin) {
                return scaleMin; // Handle case where all values are the same
            }
            return (x - scaleMin) / (scaleMax - scaleMin); // Normalized to [0, 1]
        };

        CheckTemporaryVertexBuffer(requiredVertices);
        CheckTemporaryIndexBuffer(requiredIndices);

        size_t pointCount = count * 2;
        size_t vertexIndex = 0;
        size_t indiceIndex = 0;

        for(size_t i = 0; i < valuesCount -1; i++) 
		{
            float x1 = position.x + ((i+0) * step);
            float x2 = position.x + ((i+1) * step);
            float y1 = position.y + (normalize(data[i], scaleMin, scaleMax) * plotHeight);
            float y2 = position.y + (normalize(data[i+1], scaleMin, scaleMax) * plotHeight);

            Vector2 p1(x1, y1);
            Vector2 p2(x2, y2);

            Vector2 direction(p2.x - p1.x, p2.y - p1.y);

            float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

            if (length == 0) 
                return;

            direction.x /= length;
            direction.y /= length;

            Vector2 perpendicular(-direction.y * thickness * 0.5f, 
                                    direction.x * thickness * 0.5f);

            vertexBufferTemp[vertexIndex+0] = { Vector2(p1.x + perpendicular.x, p1.y + perpendicular.y), Vector2(0, 1), color };
            vertexBufferTemp[vertexIndex+1] = { Vector2(p1.x - perpendicular.x, p1.y - perpendicular.y), Vector2(0, 0), color };
            vertexBufferTemp[vertexIndex+2] = { Vector2(p2.x - perpendicular.x, p2.y - perpendicular.y), Vector2(1, 0), color };
            vertexBufferTemp[vertexIndex+3] = { Vector2(p2.x + perpendicular.x, p2.y + perpendicular.y), Vector2(1, 1), color };

            indexBufferTemp[indiceIndex+0] = 0 + vertexIndex;
            indexBufferTemp[indiceIndex+1] = 2 + vertexIndex;
            indexBufferTemp[indiceIndex+2] = 1 + vertexIndex;
            indexBufferTemp[indiceIndex+3] = 0 + vertexIndex;
            indexBufferTemp[indiceIndex+4] = 3 + vertexIndex;
            indexBufferTemp[indiceIndex+5] = 2 + vertexIndex;

            vertexIndex += 4;
            indiceIndex += 6;
        }

        DrawCommand command;
        command.vertices = vertexBufferTemp.data();
        command.indices = indexBufferTemp.data();
		command.numVertices = vertexIndex;
		command.numIndices = indiceIndex;
		command.textureId = textureId;
		command.textureIsFont = false;
		command.fontHasSDF = false;
		command.shaderId = shaderId;
		command.clippingRect = clippingRect;
		command.userData = userData;

        AddVertices(&command);
	}

	void Graphics2D::AddImage(const Vector2 &position, const Vector2 &size, float rotationDegrees, uint32_t textureId, const Color &color, Vector2 uv0, Vector2 uv1, const Rectangle clippingRect, uint32_t shaderId, void *userData)
	{
        Vector2 uvTopLeft = Vector2(uv0.x, uv0.y);
        Vector2 uvBottomLeft = Vector2(uv0.x, uv1.y);
        Vector2 uvBottomRight = Vector2(uv1.x, uv1.y);
        Vector2 uvTopRight = Vector2(uv1.x, uv0.y);

        Vertex2D vertices[4] = {
            { Vector2(position.x, position.y), uvTopLeft, color }, // top left
            { Vector2(position.x, position.y + size.y), uvBottomLeft, color }, // bottom left
            { Vector2(position.x + size.x, position.y + size.y), uvBottomRight, color }, // bottom right
            { Vector2(position.x + size.x, position.y), uvTopRight, color }  // top right
        };

        if(rotationDegrees != 0.0f)
            RotateVertices(vertices, 4, rotationDegrees);

        uint32_t indices[6] = {
            0, 1, 2, 
            0, 2, 3
        };

        DrawCommand command;
        command.vertices = vertices;
        command.indices = indices;
		command.numVertices = 4;
		command.numIndices = 6;
		command.textureId = textureId;
		command.textureIsFont = false;
		command.fontHasSDF = false;
		command.shaderId = shaderId;
		command.clippingRect = clippingRect;
		command.userData = userData;

        AddVertices(&command);
	}

    void Graphics2D::ParseColorsFromText(std::string& text, std::vector<TextColorInfo>& colors, size_t& count) 
    {
        size_t pos = 0;
        size_t textLength = text.length();

        while (pos < textLength) 
        {
            // Find the opening brace
            size_t start = text.find('{', pos);
            if (start == std::string::npos) break;

            // Find the closing brace
            size_t end = text.find('}', start);
            if (end == std::string::npos) break;

            // Extract the color code
            std::string colorCode = text.substr(start + 1, end - start - 1);

            // Validate the length of the color code
            if (colorCode.length() == 8) 
            {
                int r = std::stoi(colorCode.substr(0, 2), nullptr, 16);
                int g = std::stoi(colorCode.substr(2, 2), nullptr, 16);
                int b = std::stoi(colorCode.substr(4, 2), nullptr, 16);
                int a = std::stoi(colorCode.substr(6, 2), nullptr, 16);

                // Normalize the color values
                float normalizedR = r / 255.0f;
                float normalizedG = g / 255.0f;
                float normalizedB = b / 255.0f;
                float normalizedA = a / 255.0f;

                // Color buffer needs to have more capacity at this point
                if (count >= colors.size()) {
                    CheckTemporaryBuffer(colors, 1);
                }

                if (count < colors.size()) 
                {
                    colors[count++] = TextColorInfo{ start, Color(normalizedR, normalizedG, normalizedB, normalizedA) };
                }
            }

            // Erase the color code from the text
            text.erase(start, end - start + 1);

            // Update position
            pos = start; // Stay at the same position to check for more colors
            textLength = text.length(); // Update text length
        }
    }

	void Graphics2D::AddText(const Vector2 &position, Font *font, const std::string &text, float fontSize, const Color &color, bool richText, const Rectangle clippingRect)
	{
		AddText(position, font, text.c_str(), text.size(), fontSize, color, richText, clippingRect);
	}

	void Graphics2D::AddText(const Vector2 &position, Font *font, const char *text, size_t textLength, float fontSize, const Color &color, bool richText, const Rectangle clippingRect)
	{
		if(font == nullptr || text == nullptr || textLength == 0)
			return;

		size_t colorCount = 0;

		std::string currentText = std::string(text, textLength);

        auto containsBraces = [] (const std::string& text) -> bool {
            return (text.find('{') != std::string::npos) && (text.find('}') != std::string::npos);
        };

		if(richText) 
        {
			if(containsBraces(currentText))
			{
				ParseColorsFromText(currentText, textColorInfoTemp, colorCount);
			}
		}

		size_t requiredVertices = currentText.size() * 4; // 4 vertices per character
		size_t requiredIndices = currentText.size() * 6; // 6 indices per character

		//Actual vertex count may be less if new line characters are present
		CheckTemporaryVertexBuffer(requiredVertices);

		//Actual indice count may be less if new line characters are present
		CheckTemporaryIndexBuffer(requiredIndices);
		
		int vertexIndex = 0;
		int indiceIndex = 0;
		int colorIndex = 0;
		Color currentColor = color;
		
		Vector2 pos(position.x, position.y);
		pos.y += font->CalculateYOffset(fontSize);
		
		float originX = pos.x;
		float originY = pos.y;
		float scale = font->GetPixelScale(fontSize);

		for(size_t i = 0; i < currentText.size(); i++) {
			char ch = currentText[i];

			if(ch == '\n') 
			{
				pos.x = originX;
				pos.y += font->GetMaxHeight() * scale;
				continue;
			}

			const Glyph *glyph = font->GetGlyph(ch);

			if(glyph == nullptr)
				continue;

			float xpos = pos.x + glyph->bearingX * scale;
			float ypos = pos.y - glyph->bearingY * scale;
			float w = glyph->sizeX * scale;
			float h = glyph->sizeY * scale;

			// top-right, top-left, bottom-left, bottom-right
			Vector2 glyphVertices[4] = 
			{
				Vector2(xpos + w, ypos + h),
				Vector2(xpos,     ypos + h),
				Vector2(xpos,     ypos),
				Vector2(xpos + w, ypos)
			};

			Vector2 glyphTextureCoords[4] = {
				Vector2(glyph->u1,  glyph->v1),
				Vector2(glyph->u0,  glyph->v1),
				Vector2(glyph->u0,  glyph->v0),
				Vector2(glyph->u1,  glyph->v0)
			};

			if(colorCount > 0) 
			{
				if (colorIndex < colorCount && textColorInfoTemp[colorIndex].index == i) 
				{
					currentColor = textColorInfoTemp[colorIndex].color; // Update to the new color
					colorIndex++; // Move to the next color
				}
			}

			vertexBufferTemp[vertexIndex+0] = Vertex2D(Vector2(glyphVertices[0].x, glyphVertices[0].y), glyphTextureCoords[0], currentColor);
			vertexBufferTemp[vertexIndex+1] = Vertex2D(Vector2(glyphVertices[1].x, glyphVertices[1].y), glyphTextureCoords[1], currentColor);
			vertexBufferTemp[vertexIndex+2] = Vertex2D(Vector2(glyphVertices[2].x, glyphVertices[2].y), glyphTextureCoords[2], currentColor);
			vertexBufferTemp[vertexIndex+3] = Vertex2D(Vector2(glyphVertices[3].x, glyphVertices[3].y), glyphTextureCoords[3], currentColor);

			indexBufferTemp[indiceIndex+0] = (uint32_t)(0 + vertexIndex); // Bottom-right
			indexBufferTemp[indiceIndex+1] = (uint32_t)(2 + vertexIndex); // Top-left
			indexBufferTemp[indiceIndex+2] = (uint32_t)(1 + vertexIndex); // Top-right
			indexBufferTemp[indiceIndex+3] = (uint32_t)(0 + vertexIndex); // Bottom-right
			indexBufferTemp[indiceIndex+4] = (uint32_t)(3 + vertexIndex); // Bottom-left
			indexBufferTemp[indiceIndex+5] = (uint32_t)(2 + vertexIndex); // Top-left

			vertexIndex += 4;
			indiceIndex += 6;

			pos.x += glyph->advanceX * scale;
		}

		DrawCommand command;
		command.vertices = vertexBufferTemp.data();
		command.indices = indexBufferTemp.data();
		command.numVertices = vertexIndex;
		command.numIndices = indiceIndex;
		command.textureId = font->GetTexture();
		command.textureIsFont = true;
		command.fontHasSDF = (font->GetRenderMethod() == FontRenderMethod::SDF);
		command.shaderId = shaderId;
		command.clippingRect = clippingRect;
		command.userData = nullptr;

		AddVertices(&command);
	}

	void Graphics2D::AddVertices(const DrawCommand *command) 
	{
		CheckVertexBuffer(command->numVertices);
        CheckIndexBuffer(command->numIndices);
        CheckItemBuffer(1);

        memcpy(&vertices[vertexCount], &command->vertices[0], command->numVertices * sizeof(Vertex2D));

        for(size_t i = 0; i < command->numIndices; i++) {
            indices[indiceCount+i] = command->indices[i] + vertexCount;
        }

		DrawListItem drawListItem = items[itemCount];

		drawListItem.vertexCount = command->numVertices;
		drawListItem.indiceCount = command->numIndices;
		drawListItem.vertexOffset = vertexCount;
		drawListItem.indiceOffset = indiceCount;
		drawListItem.shaderId = command->shaderId == 0 ? shaderId : command->shaderId;
		drawListItem.textureId = command->textureId;
		drawListItem.textureIsFont = command->textureIsFont;
		drawListItem.fontHasSDF = command->fontHasSDF;
		drawListItem.clippingRect = command->clippingRect;
		drawListItem.userData = command->userData;

		Rectangle rect = drawListItem.clippingRect;

		if(!rect.IsZero()) 
		{
			auto viewport = Graphics::GetViewport();
			rect.y = viewport.height - rect.y - rect.height;
			drawListItem.clippingRect = rect;
		}
		
		items[itemCount] = drawListItem;

		itemCount++;
		vertexCount += command->numVertices;
		indiceCount += command->numIndices;
	}

    void Graphics2D::CheckVertexBuffer(size_t numRequiredVertices) 
	{
        size_t verticesNeeded = vertexCount + numRequiredVertices;
        
        if(verticesNeeded > vertices.size()) 
		{
            size_t newSize = vertices.size() * 2;
            while(newSize < verticesNeeded) 
			{
                newSize *= 2;
            }
            vertices.resize(newSize);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, newSize * sizeof(Vertex2D), nullptr, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }

    void Graphics2D::CheckIndexBuffer(size_t numRequiredIndices) 
	{
        size_t indicesNeeded = indiceCount + numRequiredIndices;
        
        if(indicesNeeded > indices.size()) 
		{
            size_t newSize = indices.size() * 2;
            while(newSize < indicesNeeded) 
			{
                newSize *= 2;
            }
            indices.resize(newSize);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, newSize * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
    }

    void Graphics2D::CheckItemBuffer(size_t numRequiredItems) 
	{
        size_t itemsNeeded = itemCount + numRequiredItems;

        if(itemsNeeded > items.size()) 
		{
            size_t newSize = items.size() * 2;
            while(newSize < itemsNeeded) 
			{
                newSize *= 2;
            }
            items.resize(newSize);
        }
    }

    void Graphics2D::CheckTemporaryVertexBuffer(size_t numRequiredVertices)
	{
        if(vertexBufferTemp.size() < numRequiredVertices) 
		{
            size_t newSize = vertexBufferTemp.size() * 2;
            while(newSize < numRequiredVertices) 
			{
                newSize *= 2;
            }                
            vertexBufferTemp.resize(newSize);
        }
    }

    void Graphics2D::CheckTemporaryIndexBuffer(size_t numRequiredIndices)
	{
        if(indexBufferTemp.size() < (numRequiredIndices)) 
		{
            size_t newSize = indexBufferTemp.size() * 2;
            while(newSize < numRequiredIndices) 
			{
                newSize *= 2;
            }
            indexBufferTemp.resize(newSize);
        }
    }

    void Graphics2D::RotateVertices(Vertex2D *vertices, size_t numVertices, float angleDegrees)
	{
        float centerX = 0.0f;
        float centerY = 0.0f;

        for (size_t i = 0; i < numVertices; ++i) 
		{
            centerX += vertices[i].position.x;
            centerY += vertices[i].position.y;
        }

        centerX /= numVertices;
        centerY /= numVertices;

        float radians = angleDegrees * (glm::pi<float>() / 180.0f);
        float cosAngle = cos(radians);
        float sinAngle = sin(radians);

        for (size_t i = 0; i < numVertices; ++i) 
		{
            float translatedX = vertices[i].position.x - centerX;
            float translatedY = vertices[i].position.y - centerY;

            float rotatedX = translatedX * cosAngle - translatedY * sinAngle;
            float rotatedY = translatedX * sinAngle + translatedY * cosAngle;

            vertices[i].position.x = rotatedX + centerX;
            vertices[i].position.y = rotatedY + centerY;
        }
    }

    void Graphics2D::StoreState() 
	{
        glState.depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
        glState.blendEnabled = glIsEnabled(GL_BLEND);
        glGetIntegerv(GL_BLEND_SRC, &glState.blendSrcFactor);
        glGetIntegerv(GL_BLEND_DST, &glState.blendDstFactor);
        glGetIntegerv(GL_BLEND_EQUATION, &glState.blendEquation);
        glGetIntegerv(GL_DEPTH_FUNC, &glState.depthFunc);
    }

    void Graphics2D::RestoreState() 
	{
        if (glState.depthTestEnabled)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);

        if (glState.blendEnabled)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);

        glBlendFunc(glState.blendSrcFactor, glState.blendDstFactor);
        glBlendEquation(glState.blendEquation);
        glDepthFunc(glState.depthFunc);
    }

    void Graphics2D::CreateBuffers()
	{
		constexpr size_t sizeItems = 1024;
		constexpr size_t sizeVertices = 2 << 15;
		constexpr size_t sizeIndices = 2 << 15;
		constexpr size_t sizeTextColors = 128;

        items.resize(sizeItems);
        vertices.resize(sizeVertices);
        indices.resize(sizeIndices);
        vertexBufferTemp.resize(sizeVertices);
        indexBufferTemp.resize(sizeIndices);
		textColorInfoTemp.resize(sizeTextColors);

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex2D), nullptr, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid*)offsetof(Vertex2D, position));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid*)offsetof(Vertex2D, uv));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (GLvoid*)offsetof(Vertex2D, color));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);

        glBindVertexArray(0);
    }

    static bool CheckShader(uint32_t handle, const char* desc) 
	{
        GLint status = 0, log_length = 0;
        glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &log_length);
        if ((GLboolean)status == GL_FALSE)
            fprintf(stderr, "ERROR: failed to compile %s!\n", desc);
        if (log_length > 1) 
		{
            std::vector<char> buf;
            buf.resize((int)(log_length + 1));
            glGetShaderInfoLog(handle, log_length, nullptr, &buf[0]);
            char *pBuffer = (char*)buf.data();
            fprintf(stderr, "%s\n", pBuffer);
        }

        return (GLboolean)status == GL_TRUE;
    }

    static bool CheckProgram(GLuint handle, const char* desc) 
	{
        GLint status = 0, log_length = 0;
        glGetProgramiv(handle, GL_LINK_STATUS, &status);
        glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &log_length);
        if ((GLboolean)status == GL_FALSE)
            fprintf(stderr, "ERROR: failed to link %s!n", desc);
        if (log_length > 1) 
		{
            std::vector<char> buf;
            buf.resize((int)(log_length + 1));
            glGetProgramInfoLog(handle, log_length, nullptr, &buf[0]);
            char *pBuffer = (char*)buf.data();
            fprintf(stderr, "%s\n", pBuffer);
        }
        return (GLboolean)status == GL_TRUE;
    }

    void Graphics2D::CreateShader()
	{
        std::string vertexSource = GetVertexSource();
        std::string fragmentSource = GetFragmentSource();

        const GLchar* vertex_shader[1] = {
            vertexSource.c_str()
        };

        const GLchar* fragment_shader[1] = {
            fragmentSource.c_str()
        };

        GLuint vert_handle = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vert_handle, 1, vertex_shader, nullptr);
        glCompileShader(vert_handle);
        CheckShader(vert_handle, "vertex shader");

        GLuint frag_handle = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(frag_handle, 1, fragment_shader, nullptr);
        glCompileShader(frag_handle);
        CheckShader(frag_handle, "fragment shader");

        shaderId = glCreateProgram();
        glAttachShader(shaderId, vert_handle);
        glAttachShader(shaderId, frag_handle);
        glLinkProgram(shaderId);
        CheckProgram(shaderId, "shader program");

        glDetachShader(shaderId, vert_handle);
        glDetachShader(shaderId, frag_handle);
        glDeleteShader(vert_handle);
        glDeleteShader(frag_handle);

		uniforms[Uniform_Texture] = glGetUniformLocation(shaderId, "uTexture");
		uniforms[Uniform_Resolution] = glGetUniformLocation(shaderId, "uResolution");
		uniforms[Uniform_Projection] = glGetUniformLocation(shaderId, "uProjection");
		uniforms[Uniform_IsFont] = glGetUniformLocation(shaderId, "uIsFont");
		uniforms[Uniform_FontHasSDF] = glGetUniformLocation(shaderId, "uFontHasSDF");
		uniforms[Uniform_Time] = glGetUniformLocation(shaderId, "uTime");
    }

    void Graphics2D::CreateTexture()
	{
        unsigned char textureData[16];
        memset(textureData, 255, 16);

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
    }

    std::string Graphics2D::GetVertexSource()
    {
        std::string vertexSource = R"(#version 330 core
layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec4 aColor;

uniform mat4 uProjection;
out vec2 oTexCoord;
out vec4 oColor;

void main() {
    gl_Position = uProjection * vec4(aPosition.x, aPosition.y, 0.0, 1.0);
    oTexCoord = aTexCoord;
    oColor = aColor;
})";
        return vertexSource;
    }

    std::string Graphics2D::GetFragmentSource()
    {
        std::string fragmentSource = R"(#version 330 core
uniform sampler2D uTexture;
uniform float uTime;
uniform vec2 uResolution;
uniform int uIsFont;
uniform int uFontHasSDF;

in vec2 oTexCoord;
in vec4 oColor;
out vec4 FragColor;

void main() {
    if(uIsFont > 0) {
        if(uFontHasSDF > 0) {
            vec4 sample = texture(uTexture, oTexCoord);
            float d = sample.r;
            float aaf = fwidth(d);
            float alpha = smoothstep(0.5 - aaf, 0.5 + aaf, d);
            FragColor = vec4(oColor.rgb, alpha) * oColor;
        } else {
            vec4 sample = texture(uTexture, oTexCoord);

            if(sample.r == 0.0)
                discard;

            FragColor = vec4(oColor.rgb, 1.0) * sample.r;
        }
    } else {
        FragColor = texture(uTexture, oTexCoord) * oColor;
    }
})";
        return fragmentSource;
    }
}