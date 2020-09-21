#include "kspch.h"
#include "Renderer.h"

namespace Kaimos {

	void Renderer::BeginScene()
	{
		// Takes all scene parameters and makes sure the shaders we use get the right uniforms
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray)
	{
		// VARR bound here since RenderCommands should NOT do multiple things, they are just commands (unless specifically suposed-to)
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
		vertexArray->Unbind();
	}

}