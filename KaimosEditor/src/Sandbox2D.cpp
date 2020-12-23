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

	Kaimos::FramebufferSettings fboSettings;
	fboSettings.width = 1280;
	fboSettings.height = 720;
	m_Framebuffer = Kaimos::Framebuffer::Create(fboSettings);
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
		m_Framebuffer->Bind();
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
		m_Framebuffer->Unbind();
	}
}

void Sandbox2D::OnUIRender()
{
	KS_PROFILE_FUNCTION();

	// --- Docking Space ---
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNav;

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Dockspace", (bool*)true, window_flags);
	ImGui::PopStyleVar();
	ImGui::PopStyleVar(2);

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dock_id = ImGui::GetID("MyDockspace");
		ImGui::DockSpace(dock_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	// --- Up-Menu Tab Bar
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
				Kaimos::Application::Get().CloseApp();

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	// --- Settings Floating Window ---
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


	uint fboID = m_Framebuffer->GetFBOTextureID();
	ImGui::Image((void*)fboID, ImVec2(1280, 720));

	ImGui::End();
	ImGui::End();
}

void Sandbox2D::OnEvent(Kaimos::Event& ev)
{
	m_CameraController.OnEvent(ev);
}