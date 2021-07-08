#ifndef _RENDERER_2D_
#define _RENDERER_2D_

#include "Cameras/Camera.h"

#include "Resources/Texture.h"
#include "Scene/ECS/Components.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Kaimos {

	struct QuadVertex
	{
		// --- Vertex Variables ---
		glm::vec3 Pos		= glm::vec3(0.0f);
		glm::vec3 Normal	= glm::vec3(0.0f);
		glm::vec2 TexCoord	= glm::vec2(0.0f);
		glm::vec4 Color		= glm::vec4(1.0f);
		float TexIndex		= 0.0f;
		float TilingFactor	= 1.0f;

		// --- Editor Variables ---
		int EntityID		= 0;
	};



	//A renderer is a high-level class, a full-on renderer: doesn't deals with commands such as ClearScene, deals with high-level constructs (scenes, meshes...)
	//RenderCommands should NOT do multiple things, they are just commands (unless specifically suposed-to)
	class Renderer2D // Won't deal with Storage (will have 0 storage), no static stuff, just render commands
	{
		friend struct Renderer2DData;
	public:

		// --- Public Class Methods ---
		static void Init();
		static void Shutdown();

		// --- Public Renderer Methods ---
		static void BeginScene(const CameraComponent& camera_component, const TransformComponent& transform_component);
		static void BeginScene(const Camera& camera);
		static void EndScene();
		static void Flush();

		// --- Public Drawing Methods ---
		static void DrawSprite(const glm::mat4& transform, const SpriteRendererComponent& sprite, int entity_id);

	private:

		// --- Private Drawing Methods ---
		static uint GetTextureIndex(const Ref<Texture2D>& texture);
		static void SetupVertexBuffer(const glm::mat4& transform, int entity_id, const Ref<Material>& material, uint texture_index, std::vector<QuadVertex> vertices_vector);
		
		// --- Private Renderer Methods ---
		static void StartBatch();
		static void NextBatch();

		// --- Renderer Statistics ---
		struct Statistics
		{
			uint DrawCalls = 0, QuadCount = 0, VerticesCount = 0, IndicesCount = 0;
			uint GetTotalTrianglesCount()	const { return IndicesCount/3; }
		};

	public:

		// --- Renderer Statistics Methods ---
		static void ResetStats();
		static void IncrementIndicesDrawn(uint indices_increment);
		static const Statistics GetStats();
		static const uint GetMaxQuads();
	};
}

#endif //_RENDERER_2D_
