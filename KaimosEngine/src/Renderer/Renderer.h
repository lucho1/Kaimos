#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "RenderCommand.h"
#include "Renderer/Cameras/OrthographicCamera.h"
#include "Renderer/Resources/Shader.h"

namespace Kaimos {

	class Renderer
	{
	public:

		// --- Public Class Methods ---
		static void Init();
		static void Shutdown();

		// --- Public Renderer Methods ---
		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertex_array, const glm::mat4& transformation = glm::mat4(1.0f));

	public:

		// --- Event Methods ---
		static void OnWindowResize(uint width, uint height);

		// --- Getters ---
		inline static const RendererAPI::API GetRendererAPI() { return RendererAPI::GetAPI(); }

	private:

		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix = glm::mat4(1.0f);
		};

		static ScopePtr<SceneData> s_SceneData;
	};
}

#endif //_RENDERER_H_
