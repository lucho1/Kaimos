#include "EditorLayer.h"
#include "Core/Utils/Maths/Maths.h"

#include <ImGui/imgui.h>
#include <ImGuizmo/ImGuizmo.h>

// TODO: TEMP
#include "Renderer/OpenGL/Resources/OGLShader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Scene/SceneSerializer.h"
#include "Core/Utils/PlatformUtils.h"

namespace Kaimos {

	// ----------------------- Public Class Methods -------------------------------------------------------
	EditorLayer::EditorLayer() : Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f, true), m_EditorCamera(45.0f, 1.778f, 0.1f, 10000.0f)
	{
	}



	// ----------------------- Public Layer Methods -------------------------------------------------------
	void EditorLayer::OnAttach()
	{
		KS_PROFILE_FUNCTION();

		m_CheckerTexture = Texture2D::Create("assets/textures/Checkerboard.png");
		m_LogoTexture = Texture2D::Create("assets/textures/ChernoLogo.png");

		FramebufferSettings fboSettings;
		fboSettings.FBOAttachments = { TEXTURE_FORMAT::RGBA8, TEXTURE_FORMAT::RED_INTEGER, TEXTURE_FORMAT::DEPTH};
		fboSettings.Width = 1280;
		fboSettings.Height = 720;
		m_Framebuffer = Framebuffer::Create(fboSettings);

		m_CurrentScene = CreateRef<Scene>();

		/*
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
				glm::vec3& translation = GetComponent<TransformComponent>().Translation;
				//translation.x = rand() % 10 - 5.0f;
			}

			void OnUpdate(Timestep dt)
			{
				glm::vec3& translation = GetComponent<TransformComponent>().Translation;
				float speed = 10.0f;

				if (Input::IsKeyPressed(Key::A))
					translation.x -= speed * dt;
				if (Input::IsKeyPressed(Key::D))
					translation.x += speed * dt;
				if (Input::IsKeyPressed(Key::W))
					translation.y += speed * dt;
				if (Input::IsKeyPressed(Key::S))
					translation.y -= speed * dt;
			}
		};

		m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
		*/

		m_ScenePanel.SetContext(m_CurrentScene);
		SceneSerializer m_Serializer(m_CurrentScene);
		m_Serializer.Deserialize("assets/scenes/CubeScene.kaimos");
	}


	void EditorLayer::OnDetach()
	{
		KS_PROFILE_FUNCTION();
	}


	void EditorLayer::OnUpdate(Timestep dt)
	{
		KS_PROFILE_FUNCTION();

		// -- Viewport Resize --
		if (FramebufferSettings settings = m_Framebuffer->GetFBOSettings();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
			(settings.Width != (uint)m_ViewportSize.x || settings.Height != (uint)m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint)m_ViewportSize.x, (uint)m_ViewportSize.y);
			m_CameraController.SetAspectRatio(m_ViewportSize.x, m_ViewportSize.y);

			m_CurrentScene->SetViewportSize((uint)m_ViewportSize.x, (uint)m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
		}

		// -- Camera Update --
		if (m_ViewportFocused)
		{
			m_CameraController.OnUpdate(dt);
			m_EditorCamera.OnUpdate(dt);
		}

		// -- Render --
		Renderer2D::ResetStats();

		m_Framebuffer->Bind();
		RenderCommand::SetClearColor(glm::vec4(0.15f, 0.15f, 0.15f, 1.0f));
		RenderCommand::Clear();

		// Clear EntityID FBO texture (RED_INTEGER), make it -1 so that we can mouse pick (and the empty areas are -1)
		m_Framebuffer->ClearFBOTexture(1, -1);

		// Scene
		//Renderer2D::BeginScene(m_CameraController.GetCamera());
		//Renderer2D::DrawQuad(glm::vec3(0.0f, 0.0f, -0.1f), glm::vec2(10.0f), m_CheckerTexture, m_BackgroundTiling, glm::vec4(1.0f));

		// -- Scene Update --
		m_CurrentScene->OnUpdateEditor(dt, m_EditorCamera);

		// -- Mouse Picking --
		// Get Mouse position with respect to the viewport boundaries
		ImVec2 mouse_pos = ImGui::GetMousePos();
		mouse_pos.x -= m_ViewportLimits[0].x;
		mouse_pos.y -= m_ViewportLimits[0].y;
		
		// Get viewport size & invert mouse Y (to make 0,0 be on bottom-left and coincide with 0,0 of FBO texture)
		glm::vec2 viewport_size = m_ViewportLimits[1] - m_ViewportLimits[0];
		mouse_pos.y = viewport_size.y - mouse_pos.y;

		if (mouse_pos.x >= 0.0f && mouse_pos.y >= 0.0f && mouse_pos.x < viewport_size.x && mouse_pos.y < viewport_size.y)
		{
			int pixel_read = m_Framebuffer->GetPixelFromFBO(1, (int)mouse_pos.x, (int)mouse_pos.y);
			m_HoveredEntity = pixel_read == -1 ? Entity() : Entity(pixel_read, m_CurrentScene.get());
		}

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

		// -- Docking Space --
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

		// -- Set Dockspace & Its minimum size --
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float original_min_size = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dock_id = ImGui::GetID("MyDockspace");
			ImGui::DockSpace(dock_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		
		style.WindowMinSize.x = original_min_size;

		// -- Upper Menu Tab Bar --
		static bool show_uidemo = false;
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New", "Ctrl+N"))
					NewScene();

				if (ImGui::MenuItem("Open...", "Ctrl+O"))
					OpenScene();

				if (ImGui::MenuItem("Save", "Ctrl+S"))
					SaveScene();

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
					SaveSceneAs();

				if (ImGui::MenuItem("Exit"))
					Application::Get().CloseApp();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::MenuItem("Show UI Demo"))
					show_uidemo = !show_uidemo;

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		
		ImGui::End();

		// -- Demo Window --
		if (show_uidemo)
			ImGui::ShowDemoWindow();


		// -- Scene Panel Rendering --
		m_ScenePanel.OnUIRender();

		// Little test for Entities (this case: square entity color)
		ImGui::Separator();
		ImGui::Begin("Settings");

		std::string hovered_entity = "None";
		if (m_HoveredEntity)
			hovered_entity = m_HoveredEntity.GetComponent<TagComponent>().Tag;

		ImGui::Text("Hovered Entity: %s", hovered_entity.c_str());

		//ImGui::ColorEdit4("Squares Color", glm::value_ptr(m_Color));
		//ImGui::SliderFloat("Background Tiling", &m_BackgroundTiling, 1.0f, 100.0f, "%.2f");

		static bool camera_lock = false;
		ImGui::Checkbox("Lock Camera Rotation", &camera_lock);
		m_EditorCamera.LockRotation(camera_lock);
		
		ImGui::Separator();


		// -- Settings Window --
		// Memory Stats
		ImGui::NewLine(); ImGui::NewLine();

		static const MemoryMetrics& m = Application::Get().GetMemoryMetrics();
		static uint allocs = m.GetAllocations(),				deallocs = m.GetDeallocations();
		static uint allocs_size = m.GetAllocationsSize(),		deallocs_size = m.GetDeallocationsSize();
		static uint current_allocs = m.GetCurrentAllocations(),	current_usage = m.GetCurrentMemoryUsage();

		static bool stop = false;
		ImGui::Checkbox("Stop", &stop);
		if (!stop)
		{
			if (m_MemoryAllocationsIndex == 90)
				m_MemoryAllocationsIndex = 0;

			m_MemoryAllocations[m_MemoryAllocationsIndex] = m.GetCurrentAllocations();
			++m_MemoryAllocationsIndex;

			allocs = m.GetAllocations();				deallocs = m.GetDeallocations();			
			allocs_size = m.GetAllocationsSize();		deallocs_size = m.GetDeallocationsSize();			
			current_allocs = m.GetCurrentAllocations();	current_usage = m.GetCurrentMemoryUsage();			
		}

		float float_mem_allocs[90];
		for (uint i = 0; i < 90; ++i)
			float_mem_allocs[i] = (float)m_MemoryAllocations[i];

		char overlay[50];
		sprintf(overlay, "Current Allocations: %i (%i MB)", current_allocs, BTOMB(current_usage));

		ImGui::PlotLines("Memory Usage", float_mem_allocs, IM_ARRAYSIZE(float_mem_allocs), 0, overlay, 0.0f, 5000.0f, ImVec2(0.0f, 100.0f));
		ImGui::PlotHistogram("Memory Usage Histogram", float_mem_allocs, IM_ARRAYSIZE(float_mem_allocs), 0, overlay, 0.0f, 5000.0f, ImVec2(0.0f, 100.0f));
		
		ImGui::NewLine();
		ImGui::Text("Allocations: %i (%i MB)", allocs, BTOMB(allocs_size));
		ImGui::Text("Deallocations: %i (%i MB)", deallocs, BTOMB(deallocs_size));
		ImGui::Text("Current Memory Usage: %i Allocated (%i MB)", current_allocs, BTOMB(current_usage));


		// Renderer Settings
		ImGui::NewLine(); ImGui::NewLine();
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

		// -- Viewport --
		ImGui::Begin("Viewport");
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->SetBlockEvents(!m_ViewportFocused && !m_ViewportHovered);
		
		// Get the position where the next window begins (including Tab Bar)
		ImVec2 viewport_offset = ImGui::GetWindowPos();
		ImVec2 max_region = ImGui::GetWindowContentRegionMax();
		ImVec2 min_region = ImGui::GetWindowContentRegionMin();
		
		// Set viewport limits
		m_ViewportLimits[0] = glm::vec2(min_region.x + viewport_offset.x, min_region.y + viewport_offset.y);
		m_ViewportLimits[1] = glm::vec2(max_region.x + viewport_offset.x, max_region.y + viewport_offset.y);

		// Get viewport size & draw fbo texture
		ImVec2 ViewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = glm::vec2(ViewportPanelSize.x, ViewportPanelSize.y);
		ImGui::Image(reinterpret_cast<void*>(m_Framebuffer->GetFBOTextureID()), ViewportPanelSize, ImVec2(0, 1), ImVec2(1, 0));		

		// -- Guizmo --
		Entity selected_entity = m_ScenePanel.GetSelectedEntity();
		if (selected_entity && m_OperationGizmo != -1 && !Input::IsKeyPressed(KEY::LEFT_ALT))
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(m_ViewportLimits[0].x, m_ViewportLimits[0].y, m_ViewportLimits[1].x - m_ViewportLimits[0].x, m_ViewportLimits[1].y - m_ViewportLimits[0].y);

			// Camera
			//Entity camera = m_CurrentScene->GetPrimaryCamera();			
			//const glm::mat4& cam_proj = camera.GetComponent<CameraComponent>().Camera.GetProjection();
			//glm::mat4 cam_view = glm::inverse(camera.GetComponent<TransformComponent>().GetTransform());

			const glm::mat4& cam_proj = m_EditorCamera.GetProjection();
			glm::mat4 cam_view = m_EditorCamera.GetViewMatrix();

			// Entity Transformation
			TransformComponent& transform = selected_entity.GetComponent<TransformComponent>();
			glm::mat4 tr_mat = transform.GetTransform();

			// Snapping
			bool snap = Input::IsKeyPressed(KEY::LEFT_CONTROL) || Input::IsKeyPressed(KEY::RIGHT_CONTROL);
			float snap_value = 0.5f;
			if (m_OperationGizmo == ImGuizmo::OPERATION::ROTATE)
				snap_value = 10.0f;

			float snap_array[3] = { snap_value, snap_value, snap_value };

			// Guizmo Manipulation
			ImGuizmo::Manipulate(glm::value_ptr(cam_view), glm::value_ptr(cam_proj), (ImGuizmo::OPERATION)m_OperationGizmo, ImGuizmo::MODE::LOCAL, glm::value_ptr(tr_mat),
				nullptr, snap ? snap_array : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Maths::DecomposeTransformation(tr_mat, translation, rotation, scale);
				

				transform.Translation = translation;
				transform.Rotation += rotation - transform.Rotation;
				transform.Scale = scale;
			}
		}


		ImGui::PopStyleVar();
		ImGui::End();
	}


	void EditorLayer::OnEvent(Event& ev)
	{
		m_CameraController.OnEvent(ev);
		m_EditorCamera.OnEvent(ev);

		EventDispatcher dispatcher(ev);
		dispatcher.Dispatch<KeyPressedEvent>(KS_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(KS_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));

		// -- Event Example --
		//KS_EDITOR_TRACE("LayerTest Event: {0}", ev);
		//if (ev.GetEventType() == EVENT_TYPE::KEY_PRESSED)
		//{
		//	KeyPressedEvent& e = (KeyPressedEvent&)ev;
		//	KS_EDITOR_TRACE("{0}", (char)e.GetKeyCode());
		//}

		//EventDispatcher dispatcher(ev);
		//dispatcher.Dispatch<KeyPressedEvent>(KS_BIND_EVENT_FN(LayerTest::OnKeyPressedEvent)); // For a "OnKeyPressedEvent()" function
	}


	
	// ----------------------- Event Methods --------------------------------------------------------------
	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& ev)
	{
		if (ev.GetMouseButton() == MOUSE::BUTTON_LEFT && m_ViewportHovered && !ImGuizmo::IsUsing() && !ImGuizmo::IsOver() && !Input::IsKeyPressed(KEY::LEFT_ALT))
			m_ScenePanel.SetSelectedEntity(m_HoveredEntity);

		return false;
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& ev)
	{
		// -- Shortcuts --
		if (ev.GetRepeatCount() > 0)
			return false;

		bool control_pressed = Input::IsKeyPressed(KEY::LEFT_CONTROL) || Input::IsKeyPressed(KEY::RIGHT_CONTROL);
		switch (ev.GetKeyCode())
		{
			case KEY::N:
				if (control_pressed) NewScene();
				break;
			case KEY::O:
				if (control_pressed) OpenScene();
				break;
			case KEY::S:
				if (control_pressed && (Input::IsKeyPressed(KEY::LEFT_SHIFT) || Input::IsKeyPressed(KEY::RIGHT_SHIFT))) SaveSceneAs();
				else if (control_pressed) SaveScene();
				break;
			// Guizmo
			case KEY::Q:
				if(!ImGuizmo::IsUsing()) m_OperationGizmo = -1;
				break;
			case KEY::W:
				if (!ImGuizmo::IsUsing()) m_OperationGizmo = ImGuizmo::OPERATION::TRANSLATE;
				break;
			case KEY::E:
				if (!ImGuizmo::IsUsing()) m_OperationGizmo = ImGuizmo::OPERATION::ROTATE;
				break;
			case KEY::R:
				if (!ImGuizmo::IsUsing()) m_OperationGizmo = ImGuizmo::OPERATION::SCALE;
				break;
		}

		return false;
	}


	
	// ----------------------- Private Editor Methods -----------------------------------------------------
	void EditorLayer::NewScene()
	{
		m_CurrentScene = CreateRef<Scene>();
		m_CurrentScene->SetViewportSize((uint)m_ViewportSize.x, (uint)m_ViewportSize.y);
		m_ScenePanel.SetContext(m_CurrentScene);
	}

	void EditorLayer::SaveScene()
	{
		SceneSerializer m_Serializer(m_CurrentScene);

		// TODO: This should be handled by a filepath class/assets class or something
		if(m_CurrentScene->GetPath().empty())
			m_Serializer.Serialize("assets/scenes/" + m_CurrentScene->GetName() + ".kaimos");
		else
			m_Serializer.Serialize(m_CurrentScene->GetPath());
	}

	void EditorLayer::SaveSceneAs()
	{
		// "filter" arg is divided in 2 by the null-terminated string (\0). The 1st is the filter name to show and the 2nd is the actual filter to use
		// So this will be shown in the filters tab as "Kaimos Scene (*.kaimos) and will filter all the .kaimos files
		std::string filepath = FileDialogs::SaveFile("Kaimos Scene (*.kaimos)\0*.kaimos\0", m_CurrentScene->GetName().c_str());
		if (!filepath.empty())
		{
			// TODO: This should be handled by a filepath class/assets class or something
			// -- Get File Name --
			size_t last_slash = filepath.find_last_of("/\\");
			last_slash = last_slash == std::string::npos ? 0 : last_slash + 1;

			size_t last_dot = filepath.rfind('.');
			last_dot = last_dot == std::string::npos ? filepath.size() : last_dot;

			// -- Set Scene Data --
			m_CurrentScene->SetPath(filepath);
			m_CurrentScene->SetName(filepath.substr(last_slash, last_dot - last_slash));

			// -- Save --
			SceneSerializer m_Serializer(m_CurrentScene);
			m_Serializer.Serialize(filepath);
		}
	}

	void EditorLayer::OpenScene()
	{
		// -- Read explanation avobe (on SaveSceneAs()) --
		std::string filepath = FileDialogs::OpenFile("Kaimos Scene (*.kaimos)\0*.kaimos\0");
		if (!filepath.empty() && filepath != m_CurrentScene->GetPath()) // TODO: Compare the relative paths or scenes ids/names! Requires filesystem or scene IDs
		{
			NewScene();
			SceneSerializer m_Serializer(m_CurrentScene);
			m_Serializer.Deserialize(filepath);
		}
	}
}
