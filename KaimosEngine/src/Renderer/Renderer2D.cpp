#include "kspch.h"
#include "Renderer2D.h"

#include "Buffer.h"
#include "Resources/Shader.h"
#include "RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>


namespace Kaimos {

	struct QuadVertex
	{
		glm::vec3 Pos;
		glm::vec2 TexCoord;
		glm::vec4 Color;
		// TexID, ...
	};

	struct Renderer2DData
	{
		const uint MaxQuads = 10000;
		const uint MaxVertices = MaxQuads * 4;
		const uint MaxIndices = MaxQuads * 6;

		uint QuadIndicesDrawCount = 0;
		QuadVertex* QuadVBufferBase = nullptr;
		QuadVertex* QuadVBufferPtr = nullptr;

		Ref<VertexArray> QuadVArray;
		Ref<VertexBuffer> QuadVBuffer;
		Ref<Shader> ColoredTextureShader;
		Ref<Texture2D> WhiteTexture;
	};
	
	static Renderer2DData* s_Data;	// On shutdown, this is deleted, and ~VertexArray() called, freeing GPU Memory too
										// (the whole renderer has to be shutdown while we still have a context, otherwise, it will crash!)


	void Renderer2D::Init()
	{
		KS_PROFILE_FUNCTION();
		s_Data = new Renderer2DData();

		// --- Vertex Array & Buffers ---
		//Ref<VertexBuffer> m_VBuffer;
		//uint indices[6] = { 0, 1, 2, 2, 3, 0 };
		//float vertices[5 * 4] = {
		//		-0.5f,	-0.5f,	0.0f, 0.0f, 0.0f,
		//		 0.5f,	-0.5f,	0.0f, 1.0f, 0.0f,
		//		 0.5f,	 0.5f,	0.0f, 1.0f, 1.0f,
		//		-0.5f,	 0.5f,	0.0f, 0.0f, 1.0f
		//};

		Ref<IndexBuffer> m_IBuffer;
		uint* quadIndices = new uint[s_Data->MaxIndices];

		uint offset = 0;
		for (uint i = 0; i < s_Data->MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		//m_VBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
		//m_IBuffer = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint));
		
		s_Data->QuadVBufferBase = new QuadVertex[s_Data->MaxVertices];
		s_Data->QuadVArray = VertexArray::Create();
		s_Data->QuadVBuffer = VertexBuffer::Create(s_Data->MaxVertices * sizeof(QuadVertex));
		m_IBuffer = IndexBuffer::Create(quadIndices, s_Data->MaxIndices);

		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float4, "a_Color" }
		};

		//m_VBuffer->SetLayout(layout);
		//s_Data->QuadVArray->AddVertexBuffer(m_VBuffer);		
		s_Data->QuadVBuffer->SetLayout(layout);
		s_Data->QuadVArray->AddVertexBuffer(s_Data->QuadVBuffer);
		s_Data->QuadVArray->SetIndexBuffer(m_IBuffer);




		s_Data->QuadVArray->Unbind();
		m_IBuffer->Unbind();
		s_Data->QuadVBuffer->Unbind();
		delete[] quadIndices;
		
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
		KS_PROFILE_FUNCTION();

		// This is deleted here (manually), and not treated as smart pointer, waiting for the end of the program lifetime
		// because there is still some code of the graphics (OpenGL) that it has to run to free VRAM (for ex. deleting VArrays, Shaders...)
		delete s_Data;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		KS_PROFILE_FUNCTION();
		s_Data->ColoredTextureShader->Bind();
		s_Data->ColoredTextureShader->SetUMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

		s_Data->QuadIndicesDrawCount = 0;
		s_Data->QuadVBufferPtr = s_Data->QuadVBufferBase;
	}

	void Renderer2D::EndScene()
	{
		KS_PROFILE_FUNCTION();

		// This cast is because uint8_t is 1 byte large, so the substraction give us the elements in terms of bytes
		uint dataSize = (uint8_t*)s_Data->QuadVBufferPtr - (uint8_t*)s_Data->QuadVBufferBase;
		s_Data->QuadVBuffer->SetData(s_Data->QuadVBufferBase, dataSize);

		Flush();
	}

	void Renderer2D::Flush()
	{
		KS_PROFILE_FUNCTION();
		s_Data->QuadVArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVArray, s_Data->QuadIndicesDrawCount);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2 size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2 size, const Ref<Texture2D> texture, float tilling, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilling, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2 size, const glm::vec4& color)
	{
		KS_PROFILE_FUNCTION();

		s_Data->QuadVBufferPtr->Pos = position;
		s_Data->QuadVBufferPtr->TexCoord = {0.0f, 0.0f };
		s_Data->QuadVBufferPtr->Color = color;
		s_Data->QuadVBufferPtr++;

		s_Data->QuadVBufferPtr->Pos = { position.x + size.x, position.y, 0.0f };
		s_Data->QuadVBufferPtr->TexCoord = { 1.0f, 0.0f };
		s_Data->QuadVBufferPtr->Color = color;
		s_Data->QuadVBufferPtr++;

		s_Data->QuadVBufferPtr->Pos = { position.x + size.x, position.y + size.y, 0.0f };
		s_Data->QuadVBufferPtr->TexCoord = { 1.0f, 1.0f };
		s_Data->QuadVBufferPtr->Color = color;
		s_Data->QuadVBufferPtr++;

		s_Data->QuadVBufferPtr->Pos = { position.x, position.y + size.y, 0.0f };
		s_Data->QuadVBufferPtr->TexCoord = { 0.0f, 1.0f };
		s_Data->QuadVBufferPtr->Color = color;
		s_Data->QuadVBufferPtr++;

		s_Data->QuadIndicesDrawCount += 6;

		//glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		//s_Data->ColoredTextureShader->SetUMat4("u_Model", transform);
		s_Data->ColoredTextureShader->SetUFloat4("u_Color", color);
		//s_Data->ColoredTextureShader->SetUFloat("u_TilingFactor", 10.0f);
		//s_Data->WhiteTexture->Bind();
		//
		//s_Data->QuadVArray->Bind();
		//RenderCommand::DrawIndexed(s_Data->QuadVArray);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2 size, const Ref<Texture2D> texture, float tilling, const glm::vec4& tintColor)
	{
		KS_PROFILE_FUNCTION();
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		
		s_Data->ColoredTextureShader->SetUMat4("u_Model", transform);
		s_Data->ColoredTextureShader->SetUFloat4("u_Color", tintColor);
		s_Data->ColoredTextureShader->SetUFloat("u_TilingFactor", tilling);
		texture->Bind();
		
		s_Data->QuadVArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVArray);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2 size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2 size, float rotation, const Ref<Texture2D> texture, float tilling, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilling, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2 size, float rotation, const glm::vec4& color)
	{
		KS_PROFILE_FUNCTION();
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1)) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		s_Data->ColoredTextureShader->SetUMat4("u_Model", transform);
		s_Data->ColoredTextureShader->SetUFloat4("u_Color", color);
		s_Data->ColoredTextureShader->SetUFloat("u_TilingFactor", 10.0f);
		s_Data->WhiteTexture->Bind();

		s_Data->QuadVArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVArray);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2 size, float rotation, const Ref<Texture2D> texture, float tilling, const glm::vec4& tintColor)
	{
		KS_PROFILE_FUNCTION();
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1)) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		s_Data->ColoredTextureShader->SetUMat4("u_Model", transform);
		s_Data->ColoredTextureShader->SetUFloat4("u_Color", tintColor);
		s_Data->ColoredTextureShader->SetUFloat("u_TilingFactor", tilling);
		texture->Bind();

		s_Data->QuadVArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVArray);
	}
}