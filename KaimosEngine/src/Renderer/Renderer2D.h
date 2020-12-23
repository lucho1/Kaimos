#ifndef _RENDERER_2D_
#define _RENDERER_2D_

#include "Cameras/OrthographicCamera.h"
#include "Resources/Texture.h"

namespace Kaimos {

	//A renderer is a high-level class, a full-on renderer (doesn't deals with commands such as ClearScene), it deals with high-level constructs (scenes, meshes...)
	//RenderCommands should NOT do multiple things, they are just commands (unless specifically suposed-to)
	class Renderer2D // Won't deal with Storage (will have 0 storage), no static stuff, just render commands
	{
		friend struct Renderer2DData;
	public:

		// --- Class Methods ---
		static void Init();
		static void Shutdown();
		
		// --- Rendering Methods ---
		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();
		static void Flush();
		
		// --- Drawing Methods ---		
		static void DrawQuad(const glm::vec3& position, const glm::vec2 size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2 size, const Ref<Texture2D> texture, float tiling = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2 size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2 size, float rotation, const Ref<Texture2D> texture, float tiling = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		// Drawing methods calling other functions
		static void DrawQuad(const glm::vec2& position, const glm::vec2 size, const glm::vec4& color)
		{
			DrawQuad({ position.x, position.y, 0.0f }, size, color);
		}

		static void DrawQuad(const glm::vec2& position, const glm::vec2 size, const Ref<Texture2D> texture, float tiling = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f))
		{
			DrawQuad({ position.x, position.y, 0.0f }, size, texture, tiling, tintColor);
		}

		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2 size, float rotation, const glm::vec4& color)
		{
			DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
		}

		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2 size, float rotation, const Ref<Texture2D> texture, float tiling = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f))
		{
			DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tiling, tintColor);
		}


	private:

		// --- Rendering Methods ---
		static void SetupVertexArray(const glm::mat4& transform, const glm::vec4& color, float texture_index = 0.0f, float texture_tiling = 1.0f);
		static void StartNewBatch();

		// --- Renderer Statistics ---
		struct Statistics
		{
			uint DrawCalls = 0;
			uint QuadCount = 0;

			uint GetTotalVerticesCount()	{ return QuadCount * 4; }
			uint GetTotalIndicesCount()		{ return QuadCount * 6; }
			uint GetTotalTrianglesCount()	{ return QuadCount * 2; }
		};

	public:

		static void ResetStats();
		static const Statistics GetStats();
		static const uint GetMaxQuads();
	};

}

#endif