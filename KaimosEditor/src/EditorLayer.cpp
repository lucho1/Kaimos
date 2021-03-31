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
	EditorLayer::EditorLayer() : Layer("EditorLayer")
	{
	}



	// ----------------------- Public Layer Methods -------------------------------------------------------
	void EditorLayer::OnAttach()
	{
		KS_PROFILE_FUNCTION();

		m_CheckerTexture = Texture2D::Create("assets/textures/Checkerboard.png");
		m_LogoTexture = Texture2D::Create("assets/textures/ChernoLogo.png");

		m_IconsArray[0] = Texture2D::Create("../KaimosEngine/res/icons/selection_icon.png");
		m_IconsArray[1] = Texture2D::Create("../KaimosEngine/res/icons/transform_icon.png");
		m_IconsArray[2] = Texture2D::Create("../KaimosEngine/res/icons/rotation_icon.png");
		m_IconsArray[3] = Texture2D::Create("../KaimosEngine/res/icons/scale_icon.png");
		m_IconsArray[4] = Texture2D::Create("../KaimosEngine/res/icons/snap_icon3.png");
		m_IconsArray[5] = Texture2D::Create("../KaimosEngine/res/icons/local_trs_icon.png");
		m_IconsArray[6] = Texture2D::Create("../KaimosEngine/res/icons/world_trs_icon.png");
		m_IconsArray[7] = Texture2D::Create("../KaimosEngine/res/icons/camera_icon.png");

		FramebufferSettings fboSettings;
		fboSettings.FBOAttachments = { TEXTURE_FORMAT::RGBA8, TEXTURE_FORMAT::RED_INTEGER, TEXTURE_FORMAT::DEPTH};
		fboSettings.Width = 1280;
		fboSettings.Height = 720;
		m_Framebuffer = Framebuffer::Create(fboSettings);

		m_CurrentScene = CreateRef<Scene>();
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
			m_CurrentScene->SetViewportSize((uint)m_ViewportSize.x, (uint)m_ViewportSize.y);
			m_EditorCamera.SetCameraViewport(m_ViewportSize.x, m_ViewportSize.y);
		}

		// -- Camera Update --
		m_EditorCamera.OnUpdate(dt, m_ViewportFocused);

		// -- Render --
		Renderer2D::ResetStats();

		m_Framebuffer->Bind();
		RenderCommand::SetClearColor(glm::vec4(0.15f, 0.15f, 0.15f, 1.0f));
		RenderCommand::Clear();

		// Clear EntityID FBO texture (RED_INTEGER), make it -1 so that we can mouse pick (and the empty areas are -1)
		m_Framebuffer->ClearFBOTexture(1, -1);

		// -- Scene Update --
		m_CurrentScene->OnUpdateEditor(dt, m_EditorCamera.GetCamera());

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

		
		// -- Show Windows Booleans --
		static bool show_toolbar = true;
		static bool show_scene_panel = true;
		static bool show_project_panel = true;
		static bool show_console_panel = true;
		static bool show_files_panel = true;
		static bool show_settings_panel = true;
		static bool show_performance_panel = true;
		static bool show_viewport_panel = true;
		static bool show_uidemo = false;
		
		// -- Upper Menu Tab Bar --
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
				ImGui::MenuItem("Toolbar", nullptr, &show_toolbar);
				ImGui::MenuItem("Scene Panel", nullptr, &show_scene_panel);
				ImGui::MenuItem("Viewport", nullptr, &show_viewport_panel);
				ImGui::MenuItem("Settings Panel", nullptr, &show_settings_panel);
				ImGui::MenuItem("Performance Panel", nullptr, &show_performance_panel);
				ImGui::MenuItem("Project Panel", nullptr, &show_project_panel);
				ImGui::MenuItem("Console Panel", nullptr, &show_console_panel);
				ImGui::MenuItem("Files Panel", nullptr, &show_files_panel);
				ImGui::MenuItem("UI Demo", nullptr, &show_uidemo);

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		
		ImGui::End();

		// -- Demo Window --
		if (show_uidemo)
			ImGui::ShowDemoWindow();

		// -- Toolbar --
		static float viewport_endpos = 500.0f;
		float left_boundary = std::max(500.0f, viewport_endpos - 100.0f);
		float right_boundary = std::min(left_boundary, 1500.0f);
		m_ToolbarPanel.OnUIRender(m_IconsArray, m_EditorCamera, right_boundary);

		// -- Scene Panel Rendering --
		if(show_scene_panel)
			m_ScenePanel.OnUIRender(show_scene_panel);

		// -- Settings Panel Rendering --
		m_SettingsPanel.OnUIRender(m_HoveredEntity, show_settings_panel, show_performance_panel);

		// -- Project & Console Panels --
		if (show_files_panel)
		{
			ImGui::Begin("Folders", &show_files_panel);
			ImGui::End();
		}

		m_ProjectPanel.OnUIRender(show_project_panel, show_console_panel);


		// -- Viewport --
		if (show_viewport_panel)
		{
			ImGui::Begin("Viewport", &show_viewport_panel);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

			//ImGui::GetCursorScreenPos().x;
			//ImGui::GetWindowPos().x;
			//ImGui::GetCursorPosX();
			//ImGui::GetWindowContentRegionWidth();

			//viewport_endpos = ImGui::GetCursorScreenPos().x + ImGui::GetWindowContentRegionWidth();

			m_ViewportFocused = ImGui::IsWindowFocused();
			m_ViewportHovered = ImGui::IsWindowHovered();
			Application::Get().GetImGuiLayer()->SetBlockEvents(!m_ViewportFocused && !m_ViewportHovered);

			// Get the position where the next window begins (including Tab Bar)
			ImVec2 viewport_offset = ImGui::GetWindowPos();
			ImVec2 max_region = ImGui::GetWindowContentRegionMax();
			ImVec2 min_region = ImGui::GetWindowContentRegionMin();
			viewport_endpos = max_region.x + ImGui::GetContentRegionAvail().x/2.0f;

			// Set viewport limits
			m_ViewportLimits[0] = glm::vec2(min_region.x + viewport_offset.x, min_region.y + viewport_offset.y);
			m_ViewportLimits[1] = glm::vec2(max_region.x + viewport_offset.x, max_region.y + viewport_offset.y);


			// Get viewport size & draw fbo texture
			ImVec2 ViewportPanelSize = ImGui::GetContentRegionAvail();
			m_ViewportSize = glm::vec2(ViewportPanelSize.x, ViewportPanelSize.y);
			ImGui::Image(reinterpret_cast<void*>(m_Framebuffer->GetFBOTextureID()), ViewportPanelSize, ImVec2(0, 1), ImVec2(1, 0));

			// -- Guizmo --
			Entity selected_entity = m_ScenePanel.GetSelectedEntity();
			if (selected_entity && m_ToolbarPanel.m_SelectedOperation != -1 && !Input::IsKeyPressed(KEY::LEFT_ALT)) // TODO: Camera IsUsing on cam rework
			{
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();
				ImGuizmo::SetRect(m_ViewportLimits[0].x, m_ViewportLimits[0].y, m_ViewportLimits[1].x - m_ViewportLimits[0].x, m_ViewportLimits[1].y - m_ViewportLimits[0].y);

				// Camera
				const glm::mat4& cam_proj = m_EditorCamera.GetCamera().GetProjection();
				glm::mat4 cam_view = m_EditorCamera.GetCamera().GetView();

				// Entity Transformation
				TransformComponent& transform = selected_entity.GetComponent<TransformComponent>();
				glm::mat4 tr_mat = transform.GetTransform();

				// Snapping
				bool snap = Input::IsKeyPressed(KEY::LEFT_CONTROL) || Input::IsKeyPressed(KEY::RIGHT_CONTROL) || m_ToolbarPanel.m_Snap;
				float snap_value = 0.5f;

				if (m_ToolbarPanel.m_SelectedOperation == ImGuizmo::OPERATION::ROTATE)
					snap_value = 10.0f;

				float snap_array[3] = { snap_value, snap_value, snap_value };

				// Guizmo Manipulation
				ImGuizmo::Manipulate(glm::value_ptr(cam_view), glm::value_ptr(cam_proj), (ImGuizmo::OPERATION)m_ToolbarPanel.m_SelectedOperation, (ImGuizmo::MODE)m_ToolbarPanel.m_WorldMode,
					glm::value_ptr(tr_mat),	nullptr, snap ? snap_array : nullptr);

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
	}


	void EditorLayer::OnEvent(Event& ev)
	{
		m_EditorCamera.OnEvent(ev);

		EventDispatcher dispatcher(ev);
		dispatcher.Dispatch<KeyPressedEvent>(KS_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<KeyReleasedEvent>(KS_BIND_EVENT_FN(EditorLayer::OnKeyReleased));
		dispatcher.Dispatch<MouseButtonPressedEvent>(KS_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}


	
	// ----------------------- Event Methods --------------------------------------------------------------
	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& ev)
	{
		if (ev.GetMouseButton() == MOUSE::BUTTON_LEFT && m_ViewportHovered && !ImGuizmo::IsUsing() && !ImGuizmo::IsOver() && !Input::IsKeyPressed(KEY::LEFT_ALT))
			m_ScenePanel.SetSelectedEntity(m_HoveredEntity);

		return false;
	}

	bool EditorLayer::OnKeyReleased(KeyReleasedEvent& ev)
	{
		if (ev.GetKeyCode() == KEY::LEFT_CONTROL && m_ToolbarPanel.m_ChangeSnap)
		{
			m_ToolbarPanel.m_Snap = false;
			m_ToolbarPanel.m_ChangeSnap = false;
		}

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
			case KEY::LEFT_CONTROL:
				if (!m_ToolbarPanel.m_Snap) { m_ToolbarPanel.m_Snap = true; m_ToolbarPanel.m_ChangeSnap = true; }
				break;
			case KEY::SPACE:
				if (!ImGuizmo::IsUsing()) m_ToolbarPanel.m_WorldMode = !m_ToolbarPanel.m_WorldMode;
				break;
			case KEY::Q:
				if(!ImGuizmo::IsUsing()) m_ToolbarPanel.m_SelectedOperation = -1;
				break;
			case KEY::W:
				if (!ImGuizmo::IsUsing()) m_ToolbarPanel.m_SelectedOperation = ImGuizmo::OPERATION::TRANSLATE;
				break;
			case KEY::E:
				if (!ImGuizmo::IsUsing()) m_ToolbarPanel.m_SelectedOperation = ImGuizmo::OPERATION::ROTATE;
				break;
			case KEY::R:
				if (!ImGuizmo::IsUsing()) m_ToolbarPanel.m_SelectedOperation = ImGuizmo::OPERATION::SCALE;
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
