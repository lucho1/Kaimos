#ifndef _SANDBOX_2D_
#define _SANDBOX_2D_

#include "Kaimos.h"

class Sandbox2D : public Kaimos::Layer
{
public:

	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	
	virtual void OnUpdate(Kaimos::Timestep dt) override;
	virtual void OnUIRender() override;
	virtual void OnEvent(Kaimos::Event& ev) override;

private:

	// TODO: TEMP
	Kaimos::OrtographicCameraController m_CameraController;
	Kaimos::Ref<Kaimos::VertexArray> m_VArray;
	
	Kaimos::Ref<Kaimos::Shader> m_Shader;
	Kaimos::Ref<Kaimos::Texture2D> m_CheckerTexture;
	glm::vec4 m_Color = { 0.2f, 0.3f, 0.8f, 1.0f };

	struct ProfileResult
	{
		const char* name = "Unknown";
		float time = 0.0f;
	};

	std::vector<ProfileResult> m_ProfileResults;
};

#endif