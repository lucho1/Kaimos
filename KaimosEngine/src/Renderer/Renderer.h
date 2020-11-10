#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "RenderCommand.h"
#include "OrthographicCamera.h"
#include "Shader.h"

namespace Kaimos {

	class Renderer
	{
	public:

		// -- Methods --
		static void Init();
		static void BeginScene(OrthographicCamera& camera);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transformation = glm::mat4(1.0f));

	public:

		// -- Getters --
		inline static const RendererAPI::API GetRendererAPI() { return RendererAPI::GetAPI(); }

	private:

		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* m_SceneData;
	};
}

#endif