#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "Foundations/RenderCommand.h"
#include "Cameras/Camera.h"

namespace Kaimos {

	class Mesh;
	class Material;
	class Shader;
	class Texture2D;

	class Light;
	class PointLight;

	class Renderer // TODO: I should review what does this does and see how it is used, because is very similar to Renderer2D
	{
	public:

		// --- Public Class Methods ---
		static void CreateRenderer();
		static void Init();
		static void Shutdown();

		// --- Public Renderer Methods ---
		static bool BeginScene(const glm::mat4& view_projection_matrix, const glm::vec3& camera_pos, const std::vector<std::pair<Ref<Light>, glm::vec3>>& dir_lights, const std::vector<std::pair<Ref<PointLight>, glm::vec3>>& point_lights);
		static void EndScene(const glm::mat4& view_matrix, const glm::mat4& projection_matrix);

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertex_array, const glm::mat4& transformation = glm::mat4(1.0f));
		
		// --- Public Renderer Serialization Methods ---
		static void SerializeRenderer();
		static void DeserializeRenderer();

	public:

		// --- Getters ---
		inline static const RendererAPI::API GetRendererAPI() { return RendererAPI::GetAPI(); }

		static uint GetMaxTextureSlots();
		static uint GetCurrentTextureSlot();

		static const glm::vec3 GetSceneColor();
		static void SetSceneColor(const glm::vec3& color);
		static const uint GetMaxDirLights();
		static const uint GetMaxPointLights();

		static bool IsSceneInPBRPipeline();
		static void SetPBRPipeline(bool pbr_pipeline);

		static uint GetEnvironmentMapID();
		static glm::ivec2 GetEnvironmentMapSize();
		static uint GetEnvironmentMapResolution();
		static std::string GetEnvironmentMapFilepath();
		static void SetEnvironmentMapFilepath(const std::string& filepath, uint environment_map_resolution);
		static void ForceEnvironmentMapRecompile(uint environment_map_resolution = 1024);
		static void RemoveEnvironmentMap();


		// --- Event Methods ---
		static void OnWindowResize(uint width, uint height);

	public:

		// --- Public Renderer Textures Methods ---
		static void ResetTextureSlotIndex();
		static void BindTextures();
		static void CheckMaterialFitsInBatch(const Ref<Material>& material, std::function<void()> NextBatchFunction);
		static uint GetTextureIndex(const Ref<Texture2D>& texture, bool is_normal, std::function<void()> NextBatchFunction);

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

		// --- Private Environment Map Methods ---
		static void CompileEnvironmentMap();

		// --- Private Renderer Materials & Shaders Methods ---
		inline static bool MaterialExists(uint material_id);
		static void CreateDefaultMaterial(uint default_mat_id = 0);
		static Ref<Material> CreateMaterialWithID(uint material_id, const std::string& name);

		static Ref<Shader> GetShader(const std::string& name);

		static void SetCubemapVertices();
		static void SetQuadVertices();
		static void RenderQuad();
		static void RenderCube();
	};
}

#endif //_RENDERER_H_
