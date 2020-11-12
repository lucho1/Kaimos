// --- Entry Point & Kaimos Header---
#include <Kaimos.h>
#include <Core/EntryPoint.h>

// Other Includes
#include "imgui.h"

//TODO: TEMP
#include "Platform/OpenGL/OpenGLShader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Sandbox2D.h"


class LayerTest : public Kaimos::Layer
{
public:

	LayerTest() : Layer("LayerTest"), m_CameraController(1280.0f/720.0f, true), m_ObjPos(0.0f)
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

		m_VArray = Kaimos::VertexArray::Create();
		m_VBuffer = Kaimos::VertexBuffer::Create(vertices, sizeof(vertices));
		m_IBuffer = Kaimos::IndexBuffer::Create(indices, sizeof(indices)/sizeof(uint));

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
		m_CameraController.OnEvent(ev);
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
		// --- UPDATE ---
		m_CameraController.OnUpdate(dt);

		// --- RENDER ---
		//A renderer is a high-level class, a full-on renderer (doesn't deals with commands such as ClearScene), it deals with high-level constructs (scenes, meshes...)
		//RenderCommands should NOT do multiple things, they are just commands (unless specifically suposed-to)
		Kaimos::RenderCommand::SetClearColor(glm::vec4(0.15f, 0.15f, 0.15f, 1.0f));
		Kaimos::RenderCommand::Clear();		
		
		// -- Initial vertices (draw) test --
		Kaimos::Renderer::BeginScene(m_CameraController.GetCamera());
		
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
					std::dynamic_pointer_cast<Kaimos::OpenGLShader>(textureShader)->UploadUniformFloat4("u_Color", { color1, 1.0f });
				else
					std::dynamic_pointer_cast<Kaimos::OpenGLShader>(textureShader)->UploadUniformFloat4("u_Color", { color2, 1.0f });

				
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

		ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_FramePadding	| ImGuiTreeNodeFlags_DefaultOpen
			| ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

		if (ImGui::TreeNodeEx("Engine Camera Settings", tree_flags))
		{
			// Get Camera Values
			bool rot_active = m_CameraController.IsRotationActive();
			float rot = m_CameraController.GetRotation();
			float rot_speed = m_CameraController.GetRotationSpeed();
			float move_speed = m_CameraController.GetMovementSpeed();
			float vec[2] = { m_CameraController.GetPosition().x, m_CameraController.GetPosition().y };
			
			float zoom = m_CameraController.GetZoomLevel();
			float AR = m_CameraController.GetAspectRatio();			

			// Rotation & Movement
			if (ImGui::Checkbox("Activate Camera Rotation", &rot_active))
				m_CameraController.SetRotationActive(rot_active);

			if (ImGui::DragFloat("Rotation", &rot, 1.0f, 0.0f, 0.0f, "%.1f"))
				m_CameraController.SetRotation(rot);
			
			if (ImGui::SliderFloat("Rotation Speed", &rot_speed, 0.1f, 500.0f, "%.2f", 3.0f))
				m_CameraController.SetRotationSpeed(rot_speed);

			if (ImGui::SliderFloat("Movement Speed", &move_speed, 0.01f, 12.0f, "%.2f", 2.0f))
				m_CameraController.SetMovementSpeed(move_speed);

			if (ImGui::DragFloat2("Position", vec, 0.05f, 0.0f, 0.0f, "%.2f"))
				m_CameraController.SetPosition({vec[0], vec[1], 0.0f});

			// Zoom & AR
			if (ImGui::SliderFloat("Zoom", &zoom, 0.25f, 10.0f, "%.2f"), -5.0f)
				m_CameraController.SetZoomLevel(zoom);

			if (ImGui::SliderFloat("Aspect Ratio", &AR, 1.0f, 2.5f, "%.2f"), 2.0f)
				m_CameraController.SetAspectRatio(AR);

			ImGui::TreePop();
		}


		ImGui::End();
	}

private:

	// --- Camera ---
	Kaimos::OrtographicCameraController m_CameraController;
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
		//PushLayer(new LayerTest());
		PushLayer(new Sandbox2D());
	}

	~EditorApp() {}
};


Kaimos::Application* Kaimos::CreateApplication()
{
	return new EditorApp();
}