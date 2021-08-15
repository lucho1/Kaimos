#ifndef _RENDERER_3D_
#define _RENDERER_3D_

#include "Core/Utils/Time/Timestep.h"
#include "Cameras/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Kaimos {

	class Material;

	struct MeshRendererComponent;
	struct Vertex
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

	struct NonPBRVertex : Vertex
	{
		float Shininess		= 0.5f;
		float Specularity	= 0.5f;

		int SpecTexIndex	= 0.0f;
	};

	struct PBRVertex : Vertex
	{
		float Roughness		= 0.5f;
		float Metallic		= 0.5f;
		float AmbientOcc	= 0.5f;

		int MetalTexIndex	= 0.0f;
		int RoughTexIndex	= 0.0f;
		int AOTexIndex		= 0.0f;
	};



	class Renderer3D
	{
		friend struct Renderer3DData;
	public:

		// --- Public Class Methods ---
		static void Init();
		static void Shutdown();

		// --- Public Renderer Methods ---
		static void BeginScene();
		static void EndScene();

		// --- Public Drawing Methods ---
		static void DrawMesh(Timestep dt, const glm::mat4& transform, MeshRendererComponent& mesh_component, int entity_id);

	private:

		// --- Private Renderer Methods ---
		static void Flush();
		static void StartBatch();
		static void NextBatch();

		static void SetBaseVertexData(Vertex* dynamic_vertex, const Vertex& mesh_vertex, const glm::mat4& transform, const Ref<Material>& material, uint albedo_ix, uint norm_ix, uint ent_id);

	private:

		// --- Renderer Statistics ---
		struct Statistics
		{
			uint DrawCalls = 0, VerticesCount = 0, IndicesCount = 0;
			uint GetTotalTrianglesCount()	const { return IndicesCount / 3; }
		};

	public:

		// --- Renderer Statistics Methods ---
		static void ResetStats();
		static const Statistics GetStats();
		static const uint GetMaxFaces();
	};
}

#endif //_RENDERER_3D_
