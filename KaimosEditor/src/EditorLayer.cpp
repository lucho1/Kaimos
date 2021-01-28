#include "EditorLayer.h"

#include <imgui.h>

// TODO: TEMP
#include "Platform/OpenGL/OpenGLShader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Kaimos {

	EditorLayer::EditorLayer() : Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f, true)
	{
	}

	void EditorLayer::OnAttach()
	{
		KS_PROFILE_FUNCTION();

		m_CheckerTexture = Texture2D::Create("assets/textures/Checkerboard.png");
		m_LogoTexture = Texture2D::Create("assets/textures/ChernoLogo.png");

		FramebufferSettings fboSettings;
		fboSettings.width = 1280;
		fboSettings.height = 720;
		m_Framebuffer = Framebuffer::Create(fboSettings);

		m_CurrentScene = CreateRef<Scene>();

		m_Entity = m_CurrentScene->CreateEntity("Square");
		m_Entity.AddComponent<SpriteRendererComponent>(glm::vec4(0.8f, 0.4f, 0.5f, 1.0f));

		m_CameraEntity = m_CurrentScene->CreateEntity("Camera");
		m_CameraEntity.AddComponent<CameraComponent>();

		// ---
		class CameraController : public ScriptableEntity
		{
		public:
			void OnDestroy() {}
			
			void OnCreate()
			{
				auto& transform = GetComponent<TransformComponent>().Transform;
				transform[3][0] = rand() % 10 - 5.0f;
			}

			void OnUpdate(Timestep dt)
			{
				//std::cout << "Timestep: " << dt << std::endl;

				auto& transform = GetComponent<TransformComponent>().Transform;
				float speed = 10.0f;

				if (Input::IsKeyPressed(KEYCODE::A))
					transform[3][0] -= speed * dt;
				if (Input::IsKeyPressed(KEYCODE::D))
					transform[3][0] += speed * dt;
				if (Input::IsKeyPressed(KEYCODE::W))
					transform[3][1] += speed * dt;
				if (Input::IsKeyPressed(KEYCODE::S))
					transform[3][1] -= speed * dt;
			}
		};

		m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
		m_ScenePanel.SetContext(m_CurrentScene);
	}

	void EditorLayer::OnDetach()
	{
		KS_PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdate(Timestep dt)
	{
		KS_PROFILE_FUNCTION();

		// --- VIEWPORT RESIZE ---
		if (FramebufferSettings settings = m_Framebuffer->GetFBOSettings();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
			(settings.width != (uint)m_ViewportSize.x || settings.height != (uint)m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint)m_ViewportSize.x, (uint)m_ViewportSize.y);
			m_CameraController.SetAspectRatio(m_ViewportSize.x, m_ViewportSize.y);

			m_CurrentScene->SetViewportSize((uint)m_ViewportSize.x, (uint)m_ViewportSize.y);
		}

		// --- CAMERA UPDATE ---
		if (m_ViewportFocused)
			m_CameraController.OnUpdate(dt);

		// --- RENDER ---
		Renderer2D::ResetStats();

		m_Framebuffer->Bind();
		RenderCommand::SetClearColor(glm::vec4(0.15f, 0.15f, 0.15f, 1.0f));
		RenderCommand::Clear();

		// -- Scene --
		//Renderer2D::BeginScene(m_CameraController.GetCamera());
		//Renderer2D::DrawQuad(glm::vec3(0.0f, 0.0f, -0.1f), glm::vec2(10.0f), m_CheckerTexture, m_BackgroundTiling, glm::vec4(1.0f));

		// --- SCENE UPDATE ---
		m_CurrentScene->OnUpdate(dt);

		/*Renderer2D::DrawQuad(glm::vec2(1.5f, -2.5f), glm::vec2(0.5f, 0.75f), { 0.3f, 0.2f, 0.8f, 1.0f });
		Renderer2D::DrawQuad(glm::vec2(0.5f, -0.5f), glm::vec2(0.5f, 0.75f), { 0.8f, 0.2f, 0.3f, 1.0f });
		Renderer2D::DrawQuad(glm::vec2(-2.0f, -1.5f), glm::vec2(0.5f, 0.75f), m_Color);

		Renderer2D::DrawRotatedQuad(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f), 45.0f, m_LogoTexture, 1.0f, glm::vec4(1.0f));

		static float rotation = 0.0f;
		rotation += dt * 50.0f;
		Renderer2D::DrawRotatedQuad(glm::vec3(-3.0f, 0.0f, 0.1f), glm::vec2(0.8f), rotation, { 0.2f, 0.8f, 0.3f, 1.0f });

		Renderer2D::EndScene();

		Renderer2D::BeginScene(m_CameraController.GetCamera());

		for (float y = -5.0f; y < 5.0f; y += 0.475f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.475f)
			{
				glm::vec4 color = { (x + 0.5f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.75f };
				Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
			}
		}*/

		//Renderer2D::EndScene();
		m_Framebuffer->Unbind();
	}

	void EditorLayer::OnUIRender()
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

		// --- Upper Menu Tab Bar ---
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
					Application::Get().CloseApp();

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		
		ImGui::End();

		// Scene Panel Rendering
		m_ScenePanel.OnUIRender();

		// Little test for Entities (this case: square entity color)
		ImGui::Separator();
		ImGui::Begin("Settings");

		//ImGui::ColorEdit4("Squares Color", glm::value_ptr(m_Color));
		ImGui::SliderFloat("Background Tiling", &m_BackgroundTiling, 1.0f, 100.0f, "%.2f");
		ImGui::Separator();

		// --- Renderer Settings Floating Window ---
		auto stats = Renderer2D::GetStats();

		ImGui::NewLine(); ImGui::NewLine();
		ImGui::Text("--- 2D RENDERER STATS ---");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads Drawn: %d", stats.QuadCount);
		ImGui::Text("Max Quads per Draw Call: %d", Renderer2D::GetMaxQuads());

		ImGui::NewLine();
		ImGui::Text("Vertices: %d", stats.GetTotalVerticesCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndicesCount());
		ImGui::Text("Tris: %d", stats.GetTotalTrianglesCount());

		ImGui::End();

		// --- Viewport ---
		ImGui::Begin("Viewport");
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->SetBlockEvents(!m_ViewportFocused || !m_ViewportHovered);

		ImVec2 ViewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = glm::vec2(ViewportPanelSize.x, ViewportPanelSize.y);

		ImGui::Image((ImTextureID)m_Framebuffer->GetFBOTextureID(), ViewportPanelSize, ImVec2(0, 1), ImVec2(1, 0));
		ImGui::PopStyleVar();
		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& ev)
	{
		m_CameraController.OnEvent(ev);

		// --- EVENT EXAMPLE ---
		//KS_EDITOR_TRACE("LayerTest Event: {0}", ev);
		//if (ev.GetEventType() == EVENT_TYPE::KEY_PRESSED)
		//{
		//	KeyPressedEvent& e = (KeyPressedEvent&)ev;
		//	KS_EDITOR_TRACE("{0}", (char)e.GetKeyCode());
		//}

		//EventDispatcher dispatcher(ev);
		//dispatcher.Dispatch<KeyPressedEvent>(KS_BIND_EVENT_FN(LayerTest::OnKeyPressedEvent)); // For a "OnKeyPressedEvent()" function
	}
}