#include <Kaimos.h>

#include "imgui.h"

//TEMP
#include <glm/gtc/matrix_transform.hpp>

class LayerTest : public Kaimos::Layer
{
public:

	LayerTest() : Layer("LayerTest"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPos(0.0f), m_ObjPos(0.0f)
	{
		// -- Initial vertices test --
		std::shared_ptr<Kaimos::VertexBuffer> m_VBuffer;
		std::shared_ptr<Kaimos::IndexBuffer> m_IBuffer;
		uint indices[6] = { 0, 1, 2, 2, 3, 0 };
		float vertices[3 * 4] = {
				-0.5f,	-0.5f,	0.0f,
				 0.5f,	-0.5f,	0.0f,
				 0.5f,	 0.5f,	0.0f,
				-0.5f,	 0.5f,	0.0f
		};

		m_VArray.reset(Kaimos::VertexArray::Create());
		m_VBuffer.reset(Kaimos::VertexBuffer::Create(vertices, sizeof(vertices)));
		m_IBuffer.reset(Kaimos::IndexBuffer::Create(indices, sizeof(indices)/sizeof(uint)));

		Kaimos::BufferLayout layout = {
			{ Kaimos::ShaderDataType::Float3, "a_Position" }
		};

		m_VBuffer->SetLayout(layout);
		m_VArray->AddVertexBuffer(m_VBuffer);
		m_VArray->SetIndexBuffer(m_IBuffer);

		m_VArray->Unbind();
		m_IBuffer->Unbind();
		m_VBuffer->Unbind();
	}

	virtual void OnEvent(Kaimos::Event& ev) override
	{
		//KS_EDITOR_TRACE("LayerTest Event: {0}", ev);
		//if (ev.GetEventType() == Kaimos::EVENT_TYPE::KEY_PRESSED)
		//{
		//	Kaimos::KeyPressedEvent& e = (Kaimos::KeyPressedEvent&)ev;
		//	KS_EDITOR_TRACE("{0}", (char)e.GetKeyCode());
		//}

		Kaimos::EventDispatcher dispatcher(ev);
		dispatcher.Dispatch<Kaimos::KeyPressedEvent>(KS_BIND_EVENT_FN(LayerTest::OnKeyPressedEvent));
	}

	bool OnKeyPressedEvent(Kaimos::KeyPressedEvent& ev)
	{
		if (ev.GetKeyCode() == KS_KEY_LEFT)
			m_CameraPos.x -= m_CameraSpeed;
		if (ev.GetKeyCode() == KS_KEY_RIGHT)
			m_CameraPos.x += m_CameraSpeed;
		if (ev.GetKeyCode() == KS_KEY_DOWN)
			m_CameraPos.y -= m_CameraSpeed;
		if (ev.GetKeyCode() == KS_KEY_UP)
			m_CameraPos.y += m_CameraSpeed;

		return false;
	}

	virtual void OnUpdate(Kaimos::Timestep dt) override
	{
		KS_ENGINE_TRACE("DeltaTime: {0} ms", dt.GetMilliseconds());
		//RenderCommands should NOT do multiple things, they are just commands (unless specifically suposed-to)
		Kaimos::RenderCommand::SetClearColor(glm::vec4(0.15f, 0.15f, 0.15f, 1.0f));
		Kaimos::RenderCommand::Clear();

		// -- Engine Camera Movement --
		if (Kaimos::Input::IsKeyPressed(KS_KEY_D))
			m_CameraPos.x += m_CameraSpeed * dt;
		else if (Kaimos::Input::IsKeyPressed(KS_KEY_A))
			m_CameraPos.x -= m_CameraSpeed * dt;
		else if (Kaimos::Input::IsKeyPressed(KS_KEY_W))
			m_CameraPos.y += m_CameraSpeed * dt;
		else if (Kaimos::Input::IsKeyPressed(KS_KEY_S))
			m_CameraPos.y -= m_CameraSpeed * dt;

		if (Kaimos::Input::IsKeyPressed(KS_KEY_E))
			m_CameraRotation -= m_CameraSpeed * dt;
		else if (Kaimos::Input::IsKeyPressed(KS_KEY_Q))
			m_CameraRotation += m_CameraSpeed * dt;

		m_Camera.SetPosition(m_CameraPos);
		m_Camera.SetRotation(m_CameraRotation);
		
		// -- Initial vertices (draw) test --
		//A renderer is a high-level class, a full-on renderer (doesn't deals with commands such as ClearScene), it deals with high-level constructs (scenes, meshes...)
		Kaimos::Renderer::BeginScene(m_Camera);
		Kaimos::Renderer::Submit(nullptr, m_VArray);
		
		// -- Tests --
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		for (int y = 0; y < 20; ++y)
		{
			for (int x = 0; x < 20; ++x)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Kaimos::Renderer::Submit(nullptr, m_VArray, transform);
			}
		}

		Kaimos::Renderer::EndScene();
		//Renderer::Flush() // In a separate thread in MT Engine
	}

	virtual void OnUIRender() override
	{
	}

private:

	Kaimos::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPos;
	float m_CameraRotation = 0.0f;
	float m_CameraSpeed = 4.0f;
	glm::vec3 m_ObjPos;

	std::shared_ptr<Kaimos::VertexArray> m_VArray;
};


class EditorApp : public Kaimos::Application
{
public:

	EditorApp()
	{
		PushLayer(new LayerTest());
	}

	~EditorApp() {}
};


Kaimos::Application* Kaimos::CreateApplication()
{
	return new EditorApp();
}