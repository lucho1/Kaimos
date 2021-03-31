#ifndef _SANDBOX2D_H_
#define _SANDBOX2D_H_

#include "Kaimos.h"
#include "Panels/ScenePanel.h"
#include "Panels/SettingsPanel.h"
#include "Panels/ProjectPanel.h"
#include "Panels/ToolbarPanel.h"
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

		// --- Event Methods ---
		bool OnKeyPressed(KeyPressedEvent& ev);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& ev);
		bool OnKeyReleased(KeyReleasedEvent& ev);

		// --- Private Editor Methods ---
		void NewScene();
		void OpenScene();
		void SaveScene();
		void SaveSceneAs();

	private:

		// TODO: TEMP
		// Scene
		CameraController m_EditorCamera;
		Ref<Scene> m_CurrentScene = nullptr;
		Entity m_Entity = {};
		Entity m_CameraEntity = {};

		ScenePanel m_ScenePanel = {};
		SettingsPanel m_SettingsPanel = {};
		ProjectPanel m_ProjectPanel = {};
		ToolbarPanel m_ToolbarPanel = {};

		Ref<Texture2D> m_IconsArray[8] = { 0 };



		// Guizmo
		//int m_OperationGizmo = 0;
		Entity m_HoveredEntity = {};

		// Rendering
		Ref<VertexArray> m_VArray = nullptr;
		
		Ref<Framebuffer> m_Framebuffer = nullptr;
		Ref<Shader> m_Shader = nullptr;
		Ref<Texture2D> m_CheckerTexture = nullptr;
		Ref<Texture2D> m_LogoTexture = nullptr;
		float m_BackgroundTiling = 10.0f;
		glm::vec4 m_Color = { 1.0f, 0.9f, 0.8f, 1.0f };

		// Viewport
		glm::vec2 m_ViewportSize = glm::vec2(0.0f);
		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportLimits[2] = {}; // min-max of where the viewport is (bounds)
	};
}

#endif //_SANDBOX2D_H_
