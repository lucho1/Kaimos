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
	m_LogoTexture = Kaimos::Texture2D::Create("assets/textures/ChernoLogo.png");
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
	Kaimos::Renderer2D::ResetStats();

	{
		KS_PROFILE_SCOPE("Sandbox2D::OnUpdate::RenderingPreparation");
		Kaimos::RenderCommand::SetClearColor(glm::vec4(0.15f, 0.15f, 0.15f, 1.0f));
		Kaimos::RenderCommand::Clear();
	}
	
	// -- Scene --
	{
		KS_PROFILE_SCOPE("Sandbox2D::OnUpdate::Rendering");
		Kaimos::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Kaimos::Renderer2D::DrawQuad(glm::vec2(1.5f, -2.5f), glm::vec2(0.5f, 0.75f), { 0.3f, 0.2f, 0.8f, 1.0f });
		Kaimos::Renderer2D::DrawQuad(glm::vec2(0.5f, -0.5f), glm::vec2(0.5f, 0.75f), { 0.8f, 0.2f, 0.3f, 1.0f });
		Kaimos::Renderer2D::DrawQuad(glm::vec2(-2.0f, -1.5f), glm::vec2(0.5f, 0.75f), m_Color);
		
		Kaimos::Renderer2D::DrawQuad(glm::vec3(0.0f, 0.0f, -0.1f), glm::vec2(10.0f), m_CheckerTexture, m_BackgroundTiling, glm::vec4(1.0f));
		Kaimos::Renderer2D::DrawRotatedQuad(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f), 45.0f, m_LogoTexture, 1.0f, glm::vec4(1.0f));
		
		static float rotation = 0.0f;
		rotation += dt * 50.0f;

		Kaimos::Renderer2D::DrawRotatedQuad(glm::vec3(-3.0f, 0.0f, 0.1f), glm::vec2(0.8f), rotation, { 0.2f, 0.8f, 0.3f, 1.0f });
		Kaimos::Renderer2D::EndScene();

		Kaimos::Renderer2D::BeginScene(m_CameraController.GetCamera());

		for (float y = -5.0f; y < 5.0f; y += 0.475f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.475f)
			{
				glm::vec4 color = { (x + 0.5f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.75f };
				Kaimos::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
			}
		}

		Kaimos::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnUIRender()
{
	KS_PROFILE_FUNCTION();
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Squares Color", glm::value_ptr(m_Color));
	ImGui::SliderFloat("Background Tiling", &m_BackgroundTiling, 1.0f, 100.0f, "%.2f");

	auto stats = Kaimos::Renderer2D::GetStats();

	ImGui::NewLine(); ImGui::NewLine();
	ImGui::Text("--- 2D RENDERER STATS ---");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads Drawn: %d", stats.QuadCount);
	ImGui::Text("Max Quads per Draw Call: %d", Kaimos::Renderer2D::GetMaxQuads());
	
	ImGui::NewLine();
	ImGui::Text("Vertices: %d", stats.GetTotalVerticesCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndicesCount());
	ImGui::Text("Tris: %d", stats.GetTotalTrianglesCount());

	ImGui::End();
}

void Sandbox2D::OnEvent(Kaimos::Event& ev)
{
	m_CameraController.OnEvent(ev);
}