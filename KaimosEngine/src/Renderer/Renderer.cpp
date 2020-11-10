#include "kspch.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Kaimos {

	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;

	void Renderer::Init()
	{
		RenderCommand::Init();
	}

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		// Takes all scene parameters and makes sure the shaders we use get the right uniforms
		m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transformation)
	{
		if(shader != nullptr)
			shader->Bind();
		// TODO: Upload ViewProjectionMatrix uniform here and delete if statement
		// TODO: Upload tranform here as ModelMatrix
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Model", transformation);

		// Vertex Array bound here since RenderCommands should NOT do multiple things, they are just commands (unless specifically suposed-to)
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
		vertexArray->Unbind();
	}

}