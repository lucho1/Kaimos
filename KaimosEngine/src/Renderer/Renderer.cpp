#include "kspch.h"
#include "Renderer.h"

namespace Kaimos {

	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		// Takes all scene parameters and makes sure the shaders we use get the right uniforms
		m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray)
	{
		if(shader != nullptr)
			shader->Bind();
		// TODO: Upload ViewProjectionMatrix uniform here and delete if statement

		// Vertex Array bound here since RenderCommands should NOT do multiple things, they are just commands (unless specifically suposed-to)
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
		vertexArray->Unbind();
	}

}