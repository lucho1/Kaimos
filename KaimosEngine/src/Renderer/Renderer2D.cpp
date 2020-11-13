#include "kspch.h"
#include "Renderer2D.h"

#include "Buffer.h"
#include "Resources/Shader.h"
#include "RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>


namespace Kaimos {

	struct Renderer2DStorage
	{
		Ref<VertexArray> QuadVArray;
		Ref<Shader> ColoredTextureShader;
		Ref<Texture2D> WhiteTexture;
	};
	
	static Renderer2DStorage* s_Data;	// On shutdown, this is deleted, and ~VertexArray() called, freeing GPU Memory too
										// (the whole renderer has to be shutdown while we still have a context, otherwise, it will crash!)


	void Renderer2D::Init()
	{
		s_Data = new Renderer2DStorage();

		// --- Vertex Array & Buffers ---
		Ref<VertexBuffer> m_VBuffer;
		Ref<IndexBuffer> m_IBuffer;
		uint indices[6] = { 0, 1, 2, 2, 3, 0 };
		float vertices[5 * 4] = {
				-0.5f,	-0.5f,	0.0f, 0.0f, 0.0f,
				 0.5f,	-0.5f,	0.0f, 1.0f, 0.0f,
				 0.5f,	 0.5f,	0.0f, 1.0f, 1.0f,
				-0.5f,	 0.5f,	0.0f, 0.0f, 1.0f
		};

		s_Data->QuadVArray = VertexArray::Create();
		m_VBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
		m_IBuffer = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint));
		
		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		};

		m_VBuffer->SetLayout(layout);
		s_Data->QuadVArray->AddVertexBuffer(m_VBuffer);
		s_Data->QuadVArray->SetIndexBuffer(m_IBuffer);

		s_Data->QuadVArray->Unbind();
		m_IBuffer->Unbind();
		m_VBuffer->Unbind();
		
		// --- Shader ---
		s_Data->ColoredTextureShader = Shader::Create("assets/shaders/ColoredTextureShader.glsl");
		
		// --- Texture ---
		uint whiteTextData = 0xffffffff; // Full Fs for every channel there (2x4 channels - rgba -)
		s_Data->WhiteTexture = Texture2D::Create(1, 1);
		s_Data->WhiteTexture->SetData(&whiteTextData, sizeof(whiteTextData)); // or sizeof(uint)

		s_Data->ColoredTextureShader->Bind();
		s_Data->ColoredTextureShader->SetUInt("u_Texture", 0);
	}

	void Renderer2D::Shutdown()
	{
		// This is deleted here (manually), and not treated as smart pointer, waiting for the end of the program lifetime
		// because there is still some code of the graphics (OpenGL) that it has to run to free VRAM (for ex. deleting VArrays, Shaders...)
		delete s_Data;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		s_Data->ColoredTextureShader->Bind();
		s_Data->ColoredTextureShader->SetUMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
	}

	void Renderer2D::EndScene()
	{
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2 size, float rotation, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2 size, float rotation, const Ref<Texture2D> texture)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, rotation, texture);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2 size, float rotation, const glm::vec4& color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1)) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		s_Data->ColoredTextureShader->SetUMat4("u_Model", transform);
		s_Data->ColoredTextureShader->SetUFloat4("u_Color", color);
		s_Data->WhiteTexture->Bind();

		s_Data->QuadVArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVArray);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2 size, float rotation, const Ref<Texture2D> texture)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1)) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		
		s_Data->ColoredTextureShader->SetUMat4("u_Model", transform);
		s_Data->ColoredTextureShader->SetUFloat4("u_Color", glm::vec4(1.0f));
		texture->Bind();
		
		s_Data->QuadVArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVArray);
	}
}