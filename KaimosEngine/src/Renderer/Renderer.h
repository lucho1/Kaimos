#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "Foundations/RenderCommand.h"
#include "Cameras/Camera.h"

namespace Kaimos {

	class Mesh;
	class Material;
	class Shader;
	class Texture2D;

	class Renderer // TODO: I should review what does this does and see how it is used, because is very similar to Renderer2D
	{
	public:

		// --- Public Class Methods ---
		static void CreateRenderer();
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

		// --- Public Renderer Shaders Methods ---
		static Ref<Shader> GetShader(const std::string& name);
		static const glm::vec3 GetSceneColor();
		static void SetSceneColor(const glm::vec3& color);

		// --- Public Renderer Textures Methods ---
		static void BindTextures();
		static uint GetTextureIndex(const Ref<Texture2D>& texture, std::function<void()> NextBatchFunction);

		// --- Public Renderer Materials Methods ---
		static Ref<Material> CreateMaterial(const std::string& name);
		static bool IsDefaultMaterial(uint material_id);
		
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
	};
}

#endif //_RENDERER_H_
