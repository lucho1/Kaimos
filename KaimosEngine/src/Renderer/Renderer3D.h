#ifndef _RENDERER_3D_
#define _RENDERER_3D_

#include "Core/Utils/Time/Timestep.h"
#include "Cameras/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Kaimos {

	class Light;
	class PointLight;

	struct TransformComponent;
	struct CameraComponent;
	struct MeshRendererComponent;

	struct Vertex
	{
		// --- Vertex Variables ---
		glm::vec3 Pos		= glm::vec3(0.0f);
		glm::vec3 Normal	= glm::vec3(0.0f);
		glm::vec2 TexCoord	= glm::vec2(0.0f);
		glm::vec4 Color		= glm::vec4(1.0f);
		float TexIndex		= 0.0f;

		// --- Editor Variables ---
		int EntityID		= 0;
	};



	class Renderer3D
	{
		friend struct Renderer3DData;
	public:

		// --- Public Class Methods ---
		static void Init();
		static void Shutdown();

		// --- Public Renderer Methods ---
		static void BeginScene(const glm::mat4& view_projection_matrix, const std::vector<Ref<Light>>& dir_lights, const std::vector<Ref<PointLight>>& point_lights);
		static void EndScene();

		// --- Public Drawing Methods ---
		static void DrawMesh(Timestep dt, const glm::mat4& transform, MeshRendererComponent& mesh_component, int entity_id);

	private:

		// --- Private Renderer Methods ---
		static void Flush();
		static void StartBatch();
		static void NextBatch();

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
