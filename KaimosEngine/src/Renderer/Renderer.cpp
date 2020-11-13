#include "kspch.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Renderer2D.h"

namespace Kaimos {

	ScopePtr<Renderer::SceneData> Renderer::s_SceneData = CreateScopePtr<Renderer::SceneData>();

	void Renderer::Init()
	{
		RenderCommand::Init();
		Renderer2D::Init();
	}

	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
	}

	void Renderer::BeginScene(const OrthographicCamera& camera)
	{
		// Takes all scene parameters and makes sure the shaders we use get the right uniforms
		s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transformation)
	{
		shader->Bind();
		shader->SetUMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		shader->SetUMat4("u_Model", transformation);

		// Vertex Array bound here since RenderCommands should NOT do multiple things, they are just commands (unless specifically suposed-to)
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
		//vertexArray->Unbind();
	}

	// --- Resize Event ---
	void Renderer::OnWindowResize(uint width, uint height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}
}