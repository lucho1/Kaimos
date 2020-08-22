#include <Kaimos.h>

#include "imgui.h"



#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
glm::mat4 camera(float Translate, glm::vec2 const& Rotate)
{
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.f);
	glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Translate));
	View = glm::rotate(View, Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
	View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	return Projection * View * Model;
}




class LayerTest : public Kaimos::Layer
{
public:

	LayerTest() : Layer("LayerTest")
	{
		auto cam = camera(5.0f, { 5.0f, 1.0f });
		int a = 0;
	}

	virtual void OnEvent(Kaimos::Event& ev) override
	{
		//KS_EDITOR_TRACE("LayerTest Event: {0}", ev);
		if (ev.GetEventType() == Kaimos::EVENT_TYPE::KEY_PRESSED)
		{
			Kaimos::KeyPressedEvent& e = (Kaimos::KeyPressedEvent&)ev;
			KS_EDITOR_TRACE("{0}", (char)e.GetKeyCode());
		}
	}

	virtual void OnUpdate() override
	{
		//KS_EDITOR_INFO("LayerTest Update");

		if (Kaimos::Input::IsKeyPressed(KS_KEY_SPACE))
			KS_EDITOR_TRACE("SPACE KEY PRESSED");
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello World");
		ImGui::End();
	}
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