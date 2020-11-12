#include "Sandbox2D.h"

#include "imgui.h"

// TODO: TEMP
#include "Platform/OpenGL/OpenGLShader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f, true)
{
}

void Sandbox2D::OnAttach()
{
	// --- Vertex Array & Buffers ---
	Kaimos::Ref<Kaimos::VertexBuffer> m_VBuffer;
	Kaimos::Ref<Kaimos::IndexBuffer> m_IBuffer;
	uint indices[6] = { 0, 1, 2, 2, 3, 0 };
	float vertices[3 * 4] = {
			-0.5f,	-0.5f,	0.0f,
			 0.5f,	-0.5f,	0.0f,
			 0.5f,	 0.5f,	0.0f,
			-0.5f,	 0.5f,	0.0f
	};

	m_VArray = Kaimos::VertexArray::Create();
	m_VBuffer = Kaimos::VertexBuffer::Create(vertices, sizeof(vertices));
	m_IBuffer = Kaimos::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint));
	Kaimos::BufferLayout layout = { { Kaimos::ShaderDataType::Float3, "a_Position" } };

	m_VBuffer->SetLayout(layout);
	m_VArray->AddVertexBuffer(m_VBuffer);
	m_VArray->SetIndexBuffer(m_IBuffer);

	m_VArray->Unbind();
	m_IBuffer->Unbind();
	m_VBuffer->Unbind();

	// --- Shader ---
	m_Shader = Kaimos::Shader::Create("assets/shaders/FlatColorShader.glsl");
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(Kaimos::Timestep dt)
{
	// --- UPDATE ---
	m_CameraController.OnUpdate(dt);

	// --- RENDER ---
	Kaimos::RenderCommand::SetClearColor(glm::vec4(0.15f, 0.15f, 0.15f, 1.0f));
	Kaimos::RenderCommand::Clear();
	Kaimos::Renderer::BeginScene(m_CameraController.GetCamera());

	// --- Squares Test ---
	m_Shader->Bind();
	std::dynamic_pointer_cast<Kaimos::OpenGLShader>(m_Shader)->UploadUniformFloat4("u_Color", m_Color);
	Kaimos::Renderer::Submit(m_Shader, m_VArray, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

	// -- End Scene --
	Kaimos::Renderer::EndScene();
}

void Sandbox2D::OnUIRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Squares Color", glm::value_ptr(m_Color));
	ImGui::End();
}

void Sandbox2D::OnEvent(Kaimos::Event& ev)
{
	m_CameraController.OnEvent(ev);
}