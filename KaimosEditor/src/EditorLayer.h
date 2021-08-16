#ifndef _SANDBOX2D_H_
#define _SANDBOX2D_H_

#include "Kaimos.h"
#include "Panels/ScenePanel.h"
#include "Panels/SettingsPanel.h"
#include "Panels/ProjectPanel.h"
#include "Panels/ToolbarPanel.h"
#include "Panels/MaterialEditorPanel.h"
#include "Renderer/Cameras/CameraController.h"

namespace Kaimos {

	class EditorLayer : public Layer
	{
	public:

		// --- Public Class Methods ---
		EditorLayer();
		virtual ~EditorLayer() = default;

		// --- Public Layer Methods ---
		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep dt) override;
		virtual void OnUIRender() override;
		virtual void OnEvent(Event& ev) override;

	private:

		// --- Private Editor UI Methods ---
		void ShowPrimaryCameraDisplay();
		void ShowCameraSpeedMultiplier();
		void ShowGuizmo();

		// --- Event Methods ---
		bool OnWindowDragAndDrop(WindowDragDropEvent& ev);
		bool OnKeyPressed(KeyPressedEvent& ev);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& ev);
		bool OnKeyReleased(KeyReleasedEvent& ev);
		bool OnMouseScrolled(MouseScrolledEvent& ev);

		// --- Private Editor Methods ---
		void NewSceneScreen();
		void SetSceneParameters(bool set_viewport = true);

		void OpenScene();
		void SaveScene();
		void SaveSceneAs();


	private:

		// TODO: TEMP
		// Scene
		CameraController m_EditorCamera;
		Ref<Scene> m_CurrentScene = nullptr;

		// Panels
		SettingsPanel m_SettingsPanel = {};
		ProjectPanel m_ProjectPanel = {};
		ToolbarPanel m_ToolbarPanel = {};
		ScenePanel m_ScenePanel;
		MaterialEditorPanel m_KMEPanel; // Kaimos Material Editor

		Ref<Texture2D> m_IconsArray[8] = { 0 };

		// Guizmo
		//int m_OperationGizmo = 0;
		Entity m_HoveredEntity = {};
		float m_MultiSpeedPanelAlpha = false;

		// Rendering
		Ref<VertexArray> m_VArray = nullptr;
		Ref<Shader> m_Shader = nullptr;
		
		Ref<Framebuffer> m_Framebuffer = nullptr;
		Ref<Framebuffer> m_GameFramebuffer = nullptr;
		bool m_RenderGamePanel = true, m_RenderViewport = true;

		Ref<Framebuffer> m_PrimaryCameraFramebuffer = nullptr;
		glm::ivec2 m_DefaultViewportResolution = glm::ivec2(1280, 720); // Default res 1280x720 (TODO: Make this a thing of the system itself, not hardcoded)

		// Viewport
		glm::vec2 m_ViewportSize = glm::vec2(0.0f);
		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportLimits[2] = {}; // min-max of where the viewport is (bounds)
	};
}

#endif //_SANDBOX2D_H_
