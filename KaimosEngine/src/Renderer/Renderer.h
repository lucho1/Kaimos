#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "Foundations/RenderCommand.h"
#include "Resources/Shader.h"
#include "Resources/Material.h"
#include "Cameras/Camera.h"

namespace Kaimos {

	class Mesh;
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
		
		// --- Public Renderer Serialization Methods ---
		static void SerializeRenderer();
		static void DeserializeRenderer();

	public:

		// --- Getters ---
		inline static const RendererAPI::API GetRendererAPI() { return RendererAPI::GetAPI(); }

		// --- Event Methods ---
		static void OnWindowResize(uint width, uint height);

	public:

		// --- Public Renderer Materials Methods ---
		static Ref<Material> CreateMaterial(const std::string& name);
		inline static bool IsDefaultMaterial(uint material_id);
		
		// --- Public Renderer Materials Getters ---
		static Ref<Material> GetMaterial(uint material_id);
		static Ref<Material> GetMaterialFromIndex(uint index);
		static uint GetMaterialIfExists(uint material_id);
		static uint GetMaterialsQuantity();
		
		static uint GetDefaultMaterialID();

	private:

		// --- Private Renderer Materials Methods ---
		inline static bool MaterialExists(uint material_id);
		static void CreateDefaultMaterial(uint default_mat_id = 0);
		static Ref<Material> CreateMaterialWithID(uint material_id, const std::string& name);

	private:

		struct RendererData
		{
			glm::mat4 ViewProjectionMatrix = glm::mat4(1.0f);
			std::unordered_map<uint, Ref<Material>> Materials;
			uint DefaultMaterialID = 0;
		};

		static ScopePtr<RendererData> s_RendererData;
	};
}

#endif //_RENDERER_H_
