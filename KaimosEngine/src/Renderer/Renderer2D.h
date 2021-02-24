#ifndef _RENDERER_2D_
#define _RENDERER_2D_

#include "Cameras/OrthographicCamera.h"
#include "Cameras/Camera.h"
#include "Cameras/EditorCamera.h"

#include "Resources/Texture.h"

#include "Scene/Components.h"

#include <glm/gtc/matrix_transform.hpp>

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
		static void BeginScene(const OrthographicCamera& camera); // TODO: Remove this
		static void BeginScene(const EditorCamera& camera);
		static void BeginScene(const Camera& camera, const glm::mat4& camera_transform);
		static void EndScene();
		static void QuadsFlush();
		static void LinesFlush();
		
		// --- Base Drawing Methods ---
		static void DrawLine(const glm::mat4& viewproj_mat, const glm::vec3& position, const glm::vec3& destination, float width, const glm::vec4& color = glm::vec4(1.0f));
		static void DrawSprite(const glm::mat4& transform, const SpriteRendererComponent& sprite, int entity_id);

		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entity_id = -1);
		static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D> texture, int entity_id, const glm::vec4& tintColor = glm::vec4(1.0f), float tiling = 1.0f, glm::vec2 texture_uvoffset = glm::vec2(0.0f));

		// --- Non-Rotated Quads Drawing Methods (calling Base Drawing Methods) ---
		static void DrawQuad(const glm::vec2& position, const glm::vec2 size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2 size, const glm::vec4& color);		
		static void DrawQuad(const glm::vec2& position, const glm::vec2 size, const Ref<Texture2D> texture, float tiling = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));		
		static void DrawQuad(const glm::vec3& position, const glm::vec2 size, const Ref<Texture2D> texture, float tiling = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		
		// --- Rotated Quads Drawing Methods (calling Base Drawing Methods) ---
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2 size, float rotation, const glm::vec4& color);		
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2 size, float rotation, const glm::vec4& color);		
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2 size, float rotation, const Ref<Texture2D> texture, float tiling = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));		
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2 size, float rotation, const Ref<Texture2D> texture, float tiling = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));


	private:

		// --- Rendering Methods ---
		static void SetupVertexArray(const glm::mat4& transform, const glm::vec4& color, int entity_id, float texture_index = 0.0f, float texture_tiling = 1.0f, glm::vec2 texture_uvoffset = glm::vec2(0.0f));
		static void StartBatch();
		static void NextBatch();

		// --- Renderer Statistics ---
		struct Statistics
		{
			uint QuadDrawCalls = 0;
			uint QuadCount = 0;
			uint LineDrawCalls = 0;
			uint LineCount = 0;

			uint GetTotalQuadVerticesCount()	const { return QuadCount * 4; }
			uint GetTotalQuadIndicesCount()		const { return QuadCount * 6; }
			uint GetTotalTrianglesCount()		const { return QuadCount * 2; }

			uint GetTotalLineVerticesCount()	const { return LineCount * 4; }
			uint GetTotalLineIndicesCount()		const { return LineCount * 6; }

			uint GetTotalDrawCalls()			const { return LineDrawCalls + QuadDrawCalls; }
			uint GetTotalObjectsDrawn()			const { return LineCount + QuadCount; }
			uint GetTotalVerticesCount()		const { return (QuadCount + LineCount) * 4; }
			uint GetTotalIndicesCount()			const { return (QuadCount + LineCount) * 6; }
		};

	public:

		static void ResetStats();
		static const Statistics GetStats();
		static const uint GetMaxQuads();
		static const uint GetMaxLines();
	};

}

#endif
