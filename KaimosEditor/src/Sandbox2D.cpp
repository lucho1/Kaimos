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
	m_CheckerTexture = Kaimos::Texture2D::Create("assets/textures/Checkerboard.png");
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

	Kaimos::Renderer2D::BeginScene(m_CameraController.GetCamera());
	
	// -- End Scene --
	Kaimos::Renderer2D::DrawQuad(glm::vec2(-1.0f, 0.0f), glm::vec2(0.8f), 45.0f, { 0.8f, 0.2f, 0.3f, 1.0f });
	Kaimos::Renderer2D::DrawQuad(glm::vec2(0.5f, -0.5f), glm::vec2(0.5f, 0.75f), 20.0f, { 0.2f, 0.3f, 0.8f, 1.0f });
	Kaimos::Renderer2D::DrawQuad(glm::vec3(0.2f, 0.5f, -0.1f), glm::vec2(10.0f), 0.0f, m_CheckerTexture);
	Kaimos::Renderer2D::EndScene();
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