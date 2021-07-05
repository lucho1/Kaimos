#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "Foundations/RenderCommand.h"
#include "Resources/Shader.h"
#include "Resources/Material.h"
#include "Cameras/Camera.h"

namespace Kaimos {

	class Renderer // TODO: I should review what does this does and see how it is used, because is very similar to Renderer2D
	{
	public:

		// --- Public Class Methods ---
		static void Init();
		static void Shutdown();

		// --- Public Renderer Methods ---
		static void BeginScene(const Camera& camera);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertex_array, const glm::mat4& transformation = glm::mat4(1.0f));

		// --- Public Renderer Materials Methods ---
		static void CreateDefaultMaterial();
		static Ref<Material> CreateMaterial(const std::string& name);
		static Ref<Material> GetMaterial(uint material_id);
		static uint GetMaterialsQuantity();
		static Ref<Material> GetMaterialFromIndex(uint index);

		// --- Public Renderer Serialization Methods ---
		static void SerializeRenderer();
		static void DeserializeRenderer();

	public:

		// --- Event Methods ---
		static void OnWindowResize(uint width, uint height);

		// --- Getters ---
		inline static const RendererAPI::API GetRendererAPI() { return RendererAPI::GetAPI(); }

		static Ref<Material> GetDefaultMaterial();
		static bool IsDefaultMaterial(uint material_id);

	private:

		// --- Private Renderer Methods ---
		static Ref<Material> CreateMaterialWithID(uint material_id, const std::string& name);

	private:

		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix = glm::mat4(1.0f);
			std::vector<Ref<Material>> Materials;
		};

		static ScopePtr<SceneData> s_SceneData;
	};
}

#endif //_RENDERER_H_
