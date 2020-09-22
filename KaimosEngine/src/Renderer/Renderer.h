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
		static void BeginScene(OrthographicCamera& camera);
		static void EndScene();

		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);

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