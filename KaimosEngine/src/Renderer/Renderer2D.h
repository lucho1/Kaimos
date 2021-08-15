#ifndef _RENDERER_2D_
#define _RENDERER_2D_

#include "Core/Utils/Time/Timestep.h"
#include "Cameras/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Kaimos {

	class Material;

	struct SpriteRendererComponent;
	struct QuadVertex
	{
		// --- Vertex Variables ---
		glm::vec3 Pos		= glm::vec3(0.0f);
		glm::vec3 Normal	= glm::vec3(0.0f);
		glm::vec3 Tangent	= glm::vec3(0.0f);
		glm::vec2 TexCoord	= glm::vec2(0.0f);
		glm::vec4 Color		= glm::vec4(1.0f);
		float Bumpiness		= 0.5f;

		int TexIndex		= 0.0f;
		int NormTexIndex	= 0.0f;

		// --- Editor Variables ---
		int EntityID		= 0;
	};

	struct NonPBRQuadVertex : QuadVertex
	{
		float Shininess		= 0.5f;
		float Specularity	= 0.5f;

		int SpecTexIndex	= 0.0f;
	};

	struct PBRQuadVertex : QuadVertex
	{
		float Roughness		= 0.5f;
		float Metallic		= 0.5f;
		float AmbientOcc	= 0.5f;

		int RoughTexIndex	= 0.0f;
		int MetalTexIndex	= 0.0f;
		int AOTexIndex		= 0.0f;
	};



	//A renderer is a high-level class, a full-on renderer: doesn't deals with commands such as ClearScene, deals with high-level constructs (scenes, meshes...)
	//RenderCommands should NOT do multiple things, they are just commands (unless specifically supposed-to)
	class Renderer2D // Won't deal with Storage (will have 0 storage), no static stuff, just render commands
	{
		friend struct Renderer2DData;
	public:

		// --- Public Class Methods ---
		static void Init();
		static void Shutdown();

		// --- Public Renderer Methods ---
		static void BeginScene();
		static void EndScene();

		// --- Public Drawing Methods ---
		static void DrawSprite(Timestep dt, const glm::mat4& transform, SpriteRendererComponent& sprite_component, int entity_id);

	private:

		// --- Private Renderer Methods ---
		static void Flush();
		static void StartBatch();
		static void NextBatch();

		static void SetBaseVertexData(QuadVertex* dynamic_vertex, const QuadVertex& quad_vertex, const glm::mat4& transform, const Ref<Material>& material, uint albedo_ix, uint norm_ix, uint ent_id);

		// --- Renderer Statistics ---
		struct Statistics
		{
			uint DrawCalls = 0, QuadCount = 0;
			uint GetTotalVerticesCount()	const { return QuadCount * 4; }
			uint GetTotalIndicesCount()		const { return QuadCount * 6; }
			uint GetTotalTrianglesCount()	const { return QuadCount * 2; }
		};

	public:

		// --- Renderer Statistics Methods ---
		static void ResetStats();
		static const Statistics GetStats();
		static const uint GetMaxQuads();
	};
}

#endif //_RENDERER_2D_
