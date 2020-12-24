#ifndef _SANDBOX_2D_
#define _SANDBOX_2D_

#include "Kaimos.h"

namespace Kaimos {

	class EditorLayer : public Layer
	{
	public:

		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep dt) override;
		virtual void OnUIRender() override;
		virtual void OnEvent(Event& ev) override;

	private:

		// TODO: TEMP
		OrtographicCameraController m_CameraController;
		Ref<VertexArray> m_VArray;

		Ref<Framebuffer> m_Framebuffer;
		Ref<Shader> m_Shader;
		Ref<Texture2D> m_CheckerTexture;
		Ref<Texture2D> m_LogoTexture;
		float m_BackgroundTiling = 10.0f;
		glm::vec4 m_Color = { 1.0f, 0.9f, 0.8f, 1.0f };

		glm::vec2 m_ViewportSize = glm::vec2(0.0f);
	};
}
#endif