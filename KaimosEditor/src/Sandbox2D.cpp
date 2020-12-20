#include "Sandbox2D.h"

#include <imgui.h>

// TODO: TEMP
#include "Platform/OpenGL/OpenGLShader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f, true)
{
}

void Sandbox2D::OnAttach()
{
	KS_PROFILE_FUNCTION();

	m_CheckerTexture = Kaimos::Texture2D::Create("assets/textures/Checkerboard.png");
}

void Sandbox2D::OnDetach()
{
	KS_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(Kaimos::Timestep dt)
{
	KS_PROFILE_FUNCTION();

	// --- UPDATE ---
		m_CameraController.OnUpdate(dt);

	// --- RENDER ---
	{
		KS_PROFILE_SCOPE("Sandbox2D::OnUpdate::RenderingPreparation");
		Kaimos::RenderCommand::SetClearColor(glm::vec4(0.15f, 0.15f, 0.15f, 1.0f));
		Kaimos::RenderCommand::Clear();
		Kaimos::Renderer2D::BeginScene(m_CameraController.GetCamera());
	}
	
	// -- Scene --
	{
		KS_PROFILE_SCOPE("Sandbox2D::OnUpdate::Rendering");
		//Kaimos::Renderer2D::DrawRotatedQuad(glm::vec2(-1.0f, 0.0f), glm::vec2(0.8f), 45.0f, { 0.8f, 0.2f, 0.3f, 1.0f });
		Kaimos::Renderer2D::DrawQuad(glm::vec2(1.5f, -2.5f), glm::vec2(0.5f, 0.75f), { 0.3f, 0.2f, 0.8f, 1.0f });
		Kaimos::Renderer2D::DrawQuad(glm::vec2(0.5f, -0.5f), glm::vec2(0.5f, 0.75f), { 0.8f, 0.2f, 0.3f, 1.0f });
		Kaimos::Renderer2D::DrawQuad(glm::vec2(-2.0f, -1.5f), glm::vec2(0.5f, 0.75f), m_Color);
		Kaimos::Renderer2D::DrawQuad(glm::vec3(0.2f, 0.5f, -0.1f), glm::vec2(10.0f), m_CheckerTexture, m_BackgroundTiling, m_Color);
		Kaimos::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnUIRender()
{
	KS_PROFILE_FUNCTION();
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Squares Color", glm::value_ptr(m_Color));
	ImGui::SliderFloat("Background Tiling", &m_BackgroundTiling, 1.0f, 100.0f, "%.2f");
	ImGui::End();
}

void Sandbox2D::OnEvent(Kaimos::Event& ev)
{
	m_CameraController.OnEvent(ev);
}