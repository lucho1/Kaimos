#include "EditorLayer.h"
#include "Core/Utils/Maths/Maths.h"

#include <ImGui/imgui.h>
#include <ImGuizmo/ImGuizmo.h>

// TODO: TEMP
#include "Renderer/OpenGL/Resources/OGLShader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Scene/SceneSerializer.h"
#include "Core/Resources/ResourceManager.h"
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

		m_IconsArray[0] = Texture2D::Create(INTERNAL_ICONS_PATH + std::string("selection_icon.png"));
		m_IconsArray[1] = Texture2D::Create(INTERNAL_ICONS_PATH + std::string("transform_icon.png"));
		m_IconsArray[2] = Texture2D::Create(INTERNAL_ICONS_PATH + std::string("rotation_icon.png"));
		m_IconsArray[3] = Texture2D::Create(INTERNAL_ICONS_PATH + std::string("scale_icon.png"));
		m_IconsArray[4] = Texture2D::Create(INTERNAL_ICONS_PATH + std::string("snap_icon3.png"));
		m_IconsArray[5] = Texture2D::Create(INTERNAL_ICONS_PATH + std::string("local_trs_icon.png"));
		m_IconsArray[6] = Texture2D::Create(INTERNAL_ICONS_PATH + std::string("world_trs_icon.png"));
		m_IconsArray[7] = Texture2D::Create(INTERNAL_ICONS_PATH + std::string("camera_icon.png"));

		FramebufferSettings fbo_settings;
		fbo_settings.FBOAttachments = { TEXTURE_FORMAT::RGBA8, TEXTURE_FORMAT::RED_INTEGER, TEXTURE_FORMAT::DEPTH };
		fbo_settings.Width = m_DefaultViewportResolution.x;
		fbo_settings.Height = m_DefaultViewportResolution.y;
		m_Framebuffer = Framebuffer::Create(fbo_settings);

		FramebufferSettings primcam_fbo_settings;
		primcam_fbo_settings.FBOAttachments = { TEXTURE_FORMAT::RGBA8, TEXTURE_FORMAT::DEPTH };		
		primcam_fbo_settings.Width = m_DefaultViewportResolution.x;
		primcam_fbo_settings.Height = m_DefaultViewportResolution.y;
		
		m_GameFramebuffer = Framebuffer::Create(fbo_settings);
		m_PrimaryCameraFramebuffer = Framebuffer::Create(primcam_fbo_settings);

		// -- Create & Load Scene --
		CreateScene(false);

		std::string s_path = "assets/scenes/PBRDefaultScene.kaimos";
		std::string scenep = Renderer::GetLastScene();
		std::filesystem::path fpath = scenep;

		if (!std::filesystem::exists(fpath) || scenep.find("assets") == std::string::npos)
			KS_ENGINE_ERROR("Couldn't load scene");
		else
			s_path = scenep;

		SceneSerializer m_Serializer(m_CurrentScene);
		m_Serializer.Deserialize(s_path);
		m_KMEPanel = MaterialEditorPanel(m_CurrentScene);
		m_ScenePanel = ScenePanel(m_CurrentScene, &m_KMEPanel);
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
			m_CurrentScene->GetEditorCamera().SetCameraViewport(m_ViewportSize.x, m_ViewportSize.y);
		}

		// -- Camera Update --
		glm::vec3 focus_pos = glm::vec3(0.0f, 5.0f, 9.0f);
		Entity selected_entity = m_ScenePanel.GetSelectedEntity();
		if (selected_entity)
			focus_pos = selected_entity.GetComponent<TransformComponent>().Translation;

		m_CurrentScene->GetEditorCamera().OnUpdate(dt, m_ViewportFocused, focus_pos);

		// -- Render --
		Renderer2D::ResetStats();
		Renderer3D::ResetStats();

		m_Framebuffer->Bind();
		RenderCommand::SetClearColor(glm::vec4(0.15f, 0.15f, 0.15f, 1.0f));
		RenderCommand::Clear();

		// Clear EntityID FBO texture (RED_INTEGER), make it -1 so that we can mouse pick (and the empty areas are -1)
		m_Framebuffer->ClearFBOTexture(1, -1);

		// -- Scene Update --
		m_CurrentScene->OnUpdateEditor(dt);

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

		// -- Game Rendering (Primary Camera) --
		if (m_RenderGamePanel)
		{
			// -- Render --
			Renderer2D::ResetStats();
			Renderer3D::ResetStats();

			m_GameFramebuffer->Bind();
			RenderCommand::SetClearColor(glm::vec4(0.15f, 0.15f, 0.15f, 1.0f));
			RenderCommand::Clear();
			m_CurrentScene->OnUpdateRuntime(dt);
			m_GameFramebuffer->Unbind();
		}

		// -- Primary/Selected Camera Rendering --
		if (m_SettingsPanel.ShowCameraMiniScreen && m_RenderViewport)
		{
			Entity camera = {};
			if (m_SettingsPanel.ShowCameraWhenSelected)
			{
				Entity selected = m_ScenePanel.GetSelectedEntity();
				if (selected && selected.HasComponent<CameraComponent>())
					camera = m_ScenePanel.GetSelectedEntity();
			}
			else
				camera = m_CurrentScene->GetPrimaryCamera();

			if (camera)
			{
				Renderer2D::ResetStats();
				Renderer3D::ResetStats();

				m_PrimaryCameraFramebuffer->Bind();
				RenderCommand::SetClearColor(glm::vec4(0.08f, 0.08f, 0.08f, 1.0f));
				RenderCommand::Clear();
				m_CurrentScene->RenderFromCamera(dt, camera);
				m_PrimaryCameraFramebuffer->Unbind();
			}
		}		
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
		static bool fullscreen = Application::Get().GetWindow().IsFullscreen();
		static bool show_toolbar = true;
		static bool show_scene_panel = true;
		//static bool show_project_panel = true;
		static bool show_console_panel = true;
		//static bool show_files_panel = true;
		static bool show_settings_panel = true;
		static bool show_performance_panel = true;
		static bool show_viewport_panel = true;
		static bool show_game_panel = true;
		static bool show_uidemo = false;
		
		// -- Upper Menu Tab Bar --
		bool open_new_scenepopup = false;
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Fullscreen", nullptr, &fullscreen))
					Application::Get().GetWindow().SetFullscreen(fullscreen);

				if (ImGui::MenuItem("New", "Ctrl+N"))
					open_new_scenepopup = true;

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
				ImGui::MenuItem("Game Panel", nullptr, &show_game_panel);
				ImGui::MenuItem("Material Editor", nullptr, &m_KMEPanel.ShowPanel);
				ImGui::MenuItem("Settings Panel", nullptr, &show_settings_panel);
				ImGui::MenuItem("Performance Panel", nullptr, &show_performance_panel);
				//ImGui::MenuItem("Project Panel", nullptr, &show_project_panel);
				ImGui::MenuItem("Console Panel", nullptr, &show_console_panel);
				//ImGui::MenuItem("Files Panel", nullptr, &show_files_panel);

				#if !KS_DIST
					ImGui::MenuItem("UI Demo", nullptr, &show_uidemo);
				#endif

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		
		// New Scene Popup
		if (open_new_scenepopup)
		{
			ImGui::OpenPopup("Create Scene");
			open_new_scenepopup = false;
		}

		NewSceneScreen();

		ImGui::End();

		// -- Demo Window --
		if (show_uidemo)
			ImGui::ShowDemoWindow();

		// -- Toolbar --
		static float viewport_endpos = 500.0f;
		if (show_toolbar)
		{
			float left_boundary = std::max(500.0f, viewport_endpos - 100.0f);
			float right_boundary = std::min(left_boundary, 1500.0f);
			m_ToolbarPanel.OnUIRender(m_IconsArray, m_CurrentScene->GetEditorCamera(), right_boundary);
		}
		
		// -- Scene Panel Rendering --
		if(show_scene_panel)
			m_ScenePanel.OnUIRender(show_scene_panel, m_ViewportFocused);

		// -- Settings Panel Rendering --
		m_SettingsPanel.OnUIRender(m_CurrentScene, m_HoveredEntity, show_settings_panel, show_performance_panel);

		// -- Project & Console Panels --
		//if (show_files_panel)
		//{
		//	ImGui::Begin("Folders", &show_files_panel);
		//	ImGui::End();
		//}

		bool proj_panel = false; // TODO: Temporary until we have a proper project panel
		m_ProjectPanel.OnUIRender(proj_panel, show_console_panel); //proj_panel = show_project_panel

		// -- Game Panels --
		if (show_game_panel)
		{
			if (ImGui::Begin("Game", &show_game_panel, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
				m_RenderGamePanel = true;
			else
				m_RenderGamePanel = false;


			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

			Entity camera = m_CurrentScene->GetPrimaryCamera();
			if (camera)
			{
				glm::vec2 viewport_size = { ImGui::GetWindowWidth(), ImGui::GetWindowHeight() };
				ImVec2 content_region = ImGui::GetContentRegionAvail();
				glm::vec2 cam_size = camera.GetComponent<CameraComponent>().Camera.GetViewportSize();

				float scale_x = 1.0f, scale_y = 1.0f;
				if (viewport_size.x <= cam_size.x)
					scale_x = viewport_size.x / cam_size.x;

				if (viewport_size.y <= cam_size.y)
					scale_y = viewport_size.y / cam_size.y;

				glm::vec2 size = cam_size * std::min(scale_x, scale_y * 0.9f);
				glm::vec2 pos_diff = (glm::vec2(content_region.x, content_region.y) - size) / 2.0f;

				ImGui::SetWindowPos({ ImGui::GetWindowPos().x + pos_diff.x, ImGui::GetWindowPos().y + pos_diff.y });
				ImGui::Image(reinterpret_cast<void*>(m_GameFramebuffer->GetFBOTextureID()), { size.x, size.y }, ImVec2(0, 1), ImVec2(1, 0));
			}

			ImGui::PopStyleVar();
			ImGui::End();
		}
		else
			m_RenderGamePanel = false;

		// -- Viewport --
		if (show_viewport_panel)
		{
			// -- Primary Camera Mini-Screen --
			if (ImGui::Begin("Viewport", &show_viewport_panel, ImGuiWindowFlags_NoScrollbar))
			{
				m_RenderViewport = true;
				ShowPrimaryCameraDisplay();
			}
			else
				m_RenderViewport = false;


			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
						
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
			ImVec2 viewportpanel_size = ImGui::GetContentRegionAvail();
			m_ViewportSize = glm::vec2(viewportpanel_size.x, viewportpanel_size.y);
			ImGui::Image(reinterpret_cast<void*>(m_Framebuffer->GetFBOTextureID()), viewportpanel_size, ImVec2(0, 1), ImVec2(1, 0));

			// -- Camera Speed Multiplier Modification --
			ShowCameraSpeedMultiplier();

			// -- Guizmo --
			ShowGuizmo();

			ImGui::PopStyleVar();
			ImGui::End();
		}

		// -- Material Editor Panel --
		if (m_KMEPanel.ShowPanel)
			m_KMEPanel.OnUIRender();
	}


	void EditorLayer::OnEvent(Event& ev)
	{
		m_CurrentScene->GetEditorCamera().OnEvent(ev);

		if(!ImGuizmo::IsUsing() && !ImGuizmo::IsOver())
			m_ScenePanel.OnEvent(ev);

		EventDispatcher dispatcher(ev);
		dispatcher.Dispatch<KeyPressedEvent>(KS_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<KeyReleasedEvent>(KS_BIND_EVENT_FN(EditorLayer::OnKeyReleased));
		dispatcher.Dispatch<MouseButtonPressedEvent>(KS_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
		dispatcher.Dispatch<MouseScrolledEvent>(KS_BIND_EVENT_FN(EditorLayer::OnMouseScrolled));
		dispatcher.Dispatch<WindowDragDropEvent>(KS_BIND_EVENT_FN(EditorLayer::OnWindowDragAndDrop));
	}



	// ----------------------- Private Editor UI Methods --------------------------------------------------
	void EditorLayer::ShowPrimaryCameraDisplay()
	{
		if (!m_SettingsPanel.ShowCameraMiniScreen)
			return;

		Entity camera = {};
		if (m_SettingsPanel.ShowCameraWhenSelected)
		{
			Entity selected = m_ScenePanel.GetSelectedEntity();
			if (selected && selected.HasComponent<CameraComponent>())
				camera = m_ScenePanel.GetSelectedEntity();
			else
				return;
		}
		else
			camera = m_CurrentScene->GetPrimaryCamera();


		// Mini-Screen Size
		glm::ivec2 reduced_default_res = m_DefaultViewportResolution / 8;
		ImVec2 camera_img_size = ImVec2((float)reduced_default_res.x, (float)reduced_default_res.y);

		// Mini-Screen Pos
		float mini_posX = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMin().x - camera_img_size.x * 1.45f;
		float mini_posY = ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMin().y - camera_img_size.y * 2.1f;
		static glm::vec2 size_increase = glm::vec2(0.0f);

		glm::vec2 mini_screen_pos = { mini_posX + ImGui::GetWindowSize().x, mini_posY + ImGui::GetWindowSize().y };
		mini_screen_pos -= size_increase;
		ImGui::SetNextWindowPos({ mini_screen_pos.x, mini_screen_pos.y });

		ImGuiWindowFlags w_camdisp_flags = ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		ImGui::Begin("Primary Camera Display", nullptr, w_camdisp_flags);

		// Draw Primary Camera onto Mini-Screen & Modify its size according to it
		if (camera)
		{
			// Min/Max mini-screen resolutions (TODO: Make this a thing of the system itself, not hardcoded)
			int max_x = 2048 / 8, max_y = 1280 / 8, min_x = 780 / 8, min_y = 460 / 8;
			glm::ivec2 cam_view_size = camera.GetComponent<CameraComponent>().Camera.GetViewportSize() / 8;

			cam_view_size.x = std::clamp(cam_view_size.x, min_x, max_x);
			cam_view_size.y = std::clamp(cam_view_size.y, min_y, max_y);
			size_increase = cam_view_size - reduced_default_res;

			// Set Pos, Size & Draw Camera
			camera_img_size = ImVec2((float)cam_view_size.x, (float)cam_view_size.y);
			ImGui::SetWindowSize({ camera_img_size.x + 30.0f, camera_img_size.y + 29.0f });
			ImGui::Image(reinterpret_cast<void*>(m_PrimaryCameraFramebuffer->GetFBOTextureID()), camera_img_size, ImVec2(0, 1), ImVec2(1, 0));
		}
		else
		{
			// Set size increase to 0 + If not primary camera, set window size back to normal
			size_increase = glm::vec2(0.0f);
			ImGui::SetWindowSize({ camera_img_size.x + 30.0f, camera_img_size.y + 29.0f });

			// Print a warning text in the middle
			std::string no_text_str = "No Primary Camera Selected!";
			float text_size_x = ImGui::CalcTextSize(no_text_str.c_str()).x;
			ImGui::SameLine(ImGui::GetWindowSize().x / 2.0f - text_size_x / 2.0f);
			ImGui::TextColored({ 0.8f, 0.8f, 0.2f, 1.0f }, no_text_str.c_str());
		}

		ImGui::End();
	}


	void EditorLayer::ShowCameraSpeedMultiplier()
	{
		if (m_MultiSpeedPanelAlpha > 0.0f)
		{
			m_MultiSpeedPanelAlpha -= 0.015f;

			static ImVec2 popup_size = ImVec2(938.0f, 422.0f);
			ImVec2 win_size = ImGui::GetWindowSize();
			float posX = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMin().x - popup_size.x / 2.0f - 10.0f;
			float posY = ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMin().y - popup_size.y / 2.0f - 25.0f;

			ImGui::SetNextWindowPos({ posX + win_size.x / 2.0f, posY + win_size.y / 2.0f });
			ImGui::SetNextWindowBgAlpha(m_MultiSpeedPanelAlpha);

			if (ImGui::Begin("Speed Multiplicator Overlay", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
			{
				popup_size = ImGui::GetWindowSize();
				ImGui::SetWindowFontScale(2);

				float speed_multiplier = m_CurrentScene->GetEditorCamera().GetSpeedMultiplier();
				ImGui::Text("x%.2f", speed_multiplier);
			}
			ImGui::End();
		}
	}


	void EditorLayer::ShowGuizmo()
	{
		Entity selected_entity = m_ScenePanel.GetSelectedEntity();
		if (selected_entity && m_ToolbarPanel.m_SelectedOperation != -1)
		{
			// Set Guizmo
			m_CurrentScene->GetEditorCamera().UsingGuizmo(ImGuizmo::IsOver());
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(m_ViewportLimits[0].x, m_ViewportLimits[0].y, m_ViewportLimits[1].x - m_ViewportLimits[0].x, m_ViewportLimits[1].y - m_ViewportLimits[0].y);

			// Entity Transformation
			TransformComponent& transform = selected_entity.GetComponent<TransformComponent>();
			glm::mat4 tr_mat = transform.GetTransform();

			if (!m_CurrentScene->GetEditorCamera().IsUsingLMB())
			{
				// Camera
				const glm::mat4& cam_proj = m_CurrentScene->GetEditorCamera().GetCamera().GetProjection();
				glm::mat4 cam_view = m_CurrentScene->GetEditorCamera().GetCamera().GetView();

				// Snapping
				bool snap = Input::IsKeyPressed(KEY::LEFT_CONTROL) || Input::IsKeyPressed(KEY::RIGHT_CONTROL) || m_ToolbarPanel.m_Snap;
				float snap_value = 0.5f;

				if (m_ToolbarPanel.m_SelectedOperation == ImGuizmo::OPERATION::ROTATE)
					snap_value = 10.0f;

				float snap_array[3] = { snap_value, snap_value, snap_value };

				// Guizmo Manipulation
				ImGuizmo::Manipulate(glm::value_ptr(cam_view), glm::value_ptr(cam_proj), (ImGuizmo::OPERATION)m_ToolbarPanel.m_SelectedOperation, (ImGuizmo::MODE)m_ToolbarPanel.m_WorldMode,
					glm::value_ptr(tr_mat), nullptr, snap ? snap_array : nullptr);
			}

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Maths::DecomposeTransformation(tr_mat, translation, rotation, scale);

				transform.Translation = translation;
				transform.Rotation += rotation - transform.Rotation;
				transform.Scale = scale;
			}
		}
	}


	
	// ----------------------- Event Methods --------------------------------------------------------------
	bool EditorLayer::OnWindowDragAndDrop(WindowDragDropEvent& ev)
	{
		const std::vector<const char*>& paths = ev.GetPaths();
		KS_TRACE(ev.ToString());

		for (uint i = 0; i < paths.size(); ++i)
		{
			std::filesystem::path filepath = ev.GetPaths()[i];
			if(Kaimos::Resources::IsExtensionValid(filepath.extension().string()))
				m_CurrentScene->ConvertModelIntoEntities(Kaimos::Resources::ResourceManager::CreateModel(filepath.string()));
		}		

		return false;
	}

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

	bool EditorLayer::OnMouseScrolled(MouseScrolledEvent& ev)
	{
		if (Input::IsMouseButtonPressed(MOUSE::BUTTON_RIGHT))
		{
			float current_multiplier = m_CurrentScene->GetEditorCamera().GetSpeedMultiplier();
			float scroll_pow = ev.GetYOffset() * current_multiplier / 8.0f;
			m_CurrentScene->GetEditorCamera().SetSpeedMultiplier(current_multiplier + scroll_pow);
			m_MultiSpeedPanelAlpha = 0.75f;
		}

		return false;
	}


	bool EditorLayer::OnKeyPressed(KeyPressedEvent& ev)
	{
		// -- Shortcuts --
		if (ev.GetRepeatCount() > 0 || m_CurrentScene->GetEditorCamera().IsCameraMoving())
			return false;

		bool control_pressed = Input::IsKeyPressed(KEY::LEFT_CONTROL) || Input::IsKeyPressed(KEY::RIGHT_CONTROL);
		switch (ev.GetKeyCode())
		{
			case KEY::N:
				if (control_pressed) NewSceneScreen();
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
	void EditorLayer::CreateScene(bool set_viewport, bool set_rendering_pipeline, bool pbr_pipeline, const std::string& scene_name)
	{
		if(set_rendering_pipeline)
			m_CurrentScene = CreateRef<Scene>(scene_name, pbr_pipeline);
		else
			m_CurrentScene = CreateRef<Scene>();

		SetSceneParameters(set_viewport);
		Renderer::RemoveEnvironmentMap();
	}

	void EditorLayer::NewSceneScreen()
	{
		if (ImGui::BeginPopupModal("Create Scene", NULL, ImGuiWindowFlags_NoResize))
		{
			static char scene_name[128] = "";
			ImGui::Text("Choose a Name for the new Scene:");
			ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
			bool entered_scene = ImGui::InputTextWithHint("###SceneNameInputTxt", "Scene Name", scene_name, IM_ARRAYSIZE(scene_name), ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			ImGui::Text("Scene Render Pipeline");
			static int render_pipeline = 1;
			ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
			ImGui::Combo("###scenerenderpipeline", &render_pipeline, "Non-PBR\0PBR\0\0");

			ImGui::NewLine();
			if (ImGui::Button("Create", ImVec2(55.0f, 28.25f)) || entered_scene)
				ImGui::OpenPopup("Scene Created");

			bool close_popup = false, closing_warn = false;
			if (ImGui::BeginPopupModal("Scene Created", NULL, ImGuiWindowFlags_NoResize))
			{
				const std::string scene_name_str = scene_name;
				if (scene_name_str.empty())
				{
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "A Scene without name cannot be created!");
					ImGui::NewLine(); ImGui::SameLine(ImGui::GetWindowSize().x / 2.0f - 47.0f / 2.0f);
					if (closing_warn = (ImGui::Button("Close") || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)) || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape))))
						ImGui::CloseCurrentPopup();
				}
				else
				{
					ImGui::Text("Scene '%s' Created", scene_name);
					ImGui::NewLine(); ImGui::NewLine();
					ImGui::SameLine(ImGui::GetWindowSize().x / 2.0f - 47.0f / 2.0f);
					if (ImGui::Button("Close") || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)) && !entered_scene || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape)))
					{
						close_popup = true;
						ImGui::CloseCurrentPopup();

						CreateScene(true, true, render_pipeline, scene_name_str);
						memset(scene_name, 0, sizeof(scene_name));
					}
				}

				ImGui::EndPopup();
			}

			ImGui::SameLine(ImGui::GetWindowSize().x - 75.0f);
			if (ImGui::Button("Cancel") || close_popup || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape)) && !closing_warn)
			{
				ImGui::CloseCurrentPopup();
				memset(scene_name, 0, sizeof(scene_name));
			}

			ImGui::EndPopup();
		}
	}

	void EditorLayer::SetSceneParameters(bool set_viewport)
	{
		m_KMEPanel.UnsetGraphToModify();
		m_ScenePanel.SetContext(m_CurrentScene);
		m_CurrentScene->SetGlobalCurrentScene(m_CurrentScene);
		
		if(set_viewport)
			m_CurrentScene->SetViewportSize((uint)m_ViewportSize.x, (uint)m_ViewportSize.y);
	}

	void EditorLayer::SaveScene()
	{
		// -- App Serialization --
		Application::Get().Serialize();

		// -- Scene Serialization --
		m_KMEPanel.SerializeGraphs();
		SceneSerializer m_Serializer(m_CurrentScene);

		// TODO: This should be handled by a filepath class/assets class or something
		if(m_CurrentScene->GetPath().empty())
			m_Serializer.Serialize("assets/scenes/" + m_CurrentScene->GetName() + ".kaimos");
		else
			m_Serializer.Serialize(m_CurrentScene->GetPath());

		Renderer::SetLastScene(m_CurrentScene->GetPath());

		// -- Save Editor Settings (ini files) --
		ImGui::SaveIniSettingsToDisk("imgui.ini");
	}

	void EditorLayer::SaveSceneAs()
	{
		// -- App Serialization --
		Application::Get().Serialize();

		// -- Scene Serialization --
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

			// -- Save Graphs & Scene --
			m_KMEPanel.SerializeGraphs();
			SceneSerializer m_Serializer(m_CurrentScene);
			m_Serializer.Serialize(filepath);

			Renderer::SetLastScene(m_CurrentScene->GetPath());

			// -- Save Editor Settings (ini file) --
			ImGui::SaveIniSettingsToDisk("imgui.ini");
		}
	}

	void EditorLayer::OpenScene()
	{
		// -- Read explanation avobe (on SaveSceneAs()) --
		std::string filepath = FileDialogs::OpenFile("Kaimos Scene (*.kaimos)\0*.kaimos\0");
		if (!filepath.empty() && filepath != m_CurrentScene->GetPath()) // TODO: Compare the relative paths or scenes ids/names! Requires filesystem or scene IDs
		{
			CreateScene();
			SceneSerializer m_Serializer(m_CurrentScene);
			m_Serializer.Deserialize(filepath);
		}
	}
}
