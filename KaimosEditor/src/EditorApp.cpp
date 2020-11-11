#include <Kaimos.h>

#include "imgui.h"

//TEMP
#include "Platform/OpenGL/OpenGLShader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class LayerTest : public Kaimos::Layer
{
public:

	LayerTest() : Layer("LayerTest"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPos(0.0f), m_ObjPos(0.0f)
	{
		// -- Initial vertices test --
		Kaimos::Ref<Kaimos::VertexBuffer> m_VBuffer;
		Kaimos::Ref<Kaimos::IndexBuffer> m_IBuffer;
		uint indices[6] = { 0, 1, 2, 2, 3, 0 };
		float vertices[5 * 4] = {
				-0.5f,	-0.5f,	0.0f,	0.0f, 0.0f,		// For negative X positions, UV should be 0, for positive, 1
				 0.5f,	-0.5f,	0.0f,	1.0f, 0.0f,		// If you render, on a square, the texCoords (as color = vec4(tC, 0, 1)), the colors of the square in its corners are
				 0.5f,	 0.5f,	0.0f,	1.0f, 1.0f,		// (0,0,0,1) - Black, (1,0,0,1) - Red, (1,1,0,0) - Yellow, (0,1,0,1) - Green
				-0.5f,	 0.5f,	0.0f,	0.0f, 1.0
		};

		m_VArray.reset(Kaimos::VertexArray::Create());
		m_VBuffer.reset(Kaimos::VertexBuffer::Create(vertices, sizeof(vertices)));
		m_IBuffer.reset(Kaimos::IndexBuffer::Create(indices, sizeof(indices)/sizeof(uint)));

		Kaimos::BufferLayout layout = {
			{ Kaimos::ShaderDataType::Float3, "a_Position" },
			{ Kaimos::ShaderDataType::Float2, "a_TexCoord" }
		};

		m_VBuffer->SetLayout(layout);
		m_VArray->AddVertexBuffer(m_VBuffer);
		m_VArray->SetIndexBuffer(m_IBuffer);

		m_VArray->Unbind();
		m_IBuffer->Unbind();
		m_VBuffer->Unbind();

		m_CheckerTexture = Kaimos::Texture2D::Create("assets/textures/Checkerboard.png");
		m_LogoTexture = Kaimos::Texture2D::Create("assets/textures/ChernoLogo.png");
		//m_Shader = Kaimos::Shader::Create("assets/shaders/TextureShader.glsl");
		auto textureShader = m_ShaderLibrary.Load("assets/shaders/TextureShader.glsl");

		
		textureShader->Bind();
		std::dynamic_pointer_cast<Kaimos::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0); // Second parameter asks for the TEXTURE SLOT, not ID!!! (store slots?)
	}

	virtual void OnEvent(Kaimos::Event& ev) override
	{
		//KS_EDITOR_TRACE("LayerTest Event: {0}", ev);
		//if (ev.GetEventType() == Kaimos::EVENT_TYPE::KEY_PRESSED)
		//{
		//	Kaimos::KeyPressedEvent& e = (Kaimos::KeyPressedEvent&)ev;
		//	KS_EDITOR_TRACE("{0}", (char)e.GetKeyCode());
		//}

		//Kaimos::EventDispatcher dispatcher(ev);
		//dispatcher.Dispatch<Kaimos::KeyPressedEvent>(KS_BIND_EVENT_FN(LayerTest::OnKeyPressedEvent)); // For a "OnKeyPressedEvent()" function
	}

	virtual void OnUpdate(Kaimos::Timestep dt) override
	{
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
		
		// -- Tests --
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
		auto textureShader = m_ShaderLibrary.Get("TextureShader");
		textureShader->Bind();

		for (int y = 0; y < 20; ++y)
		{
			for (int x = 0; x < 20; ++x)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				
				if (x % 2 == 0)
					std::dynamic_pointer_cast<Kaimos::OpenGLShader>(textureShader)->UploadUniformFloat4("u_BaseColor", { color1, 1.0f });
				else
					std::dynamic_pointer_cast<Kaimos::OpenGLShader>(textureShader)->UploadUniformFloat4("u_BaseColor", { color2, 1.0f });

				
				Kaimos::Renderer::Submit(textureShader, m_VArray, transform);
			}
		}

		m_CheckerTexture->Bind();
		Kaimos::Renderer::Submit(textureShader, m_VArray, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		m_LogoTexture->Bind();
		Kaimos::Renderer::Submit(textureShader, m_VArray, glm::translate(glm::mat4(1.0f), glm::vec3(0.25f, -0.25f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		// -- End Scene --
		Kaimos::Renderer::EndScene();
		//Renderer::Flush() // In a separate thread in MT Engine
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Color1", glm::value_ptr(color1));
		ImGui::ColorEdit3("Color2", glm::value_ptr(color2));
		ImGui::End();
	}

private:

	// --- Camera ---
	Kaimos::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPos;
	float m_CameraRotation = 0.0f;
	float m_CameraSpeed = 4.0f;
	glm::vec3 m_ObjPos;

	// --- UI ---
	glm::vec3 color1 = { 0.8f, 0.2f, 0.3f };
	glm::vec3 color2 = { 0.2f, 0.3f, 0.8f };

	// --- Rendering ---
	Kaimos::Ref<Kaimos::VertexArray> m_VArray;
	Kaimos::ShaderLibrary m_ShaderLibrary;
	Kaimos::Ref<Kaimos::Texture2D> m_CheckerTexture;
	Kaimos::Ref<Kaimos::Texture2D> m_LogoTexture;
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