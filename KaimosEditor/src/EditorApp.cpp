#include <Kaimos.h>

class LayerTest : public Kaimos::Layer
{
public:

	LayerTest() : Layer("LayerTest") {}

	void OnEvent(Kaimos::Event& ev) override
	{
		//KS_EDITOR_TRACE("LayerTest Event: {0}", ev);
		if (ev.GetEventType() == Kaimos::EVENT_TYPE::KEY_PRESSED)
		{
			Kaimos::KeyPressedEvent& e = (Kaimos::KeyPressedEvent&)ev;
			KS_EDITOR_TRACE("{0}", (char)e.GetKeyCode());
		}
	}

	void OnUpdate() override
	{
		//KS_EDITOR_INFO("LayerTest Update");

		if (Kaimos::Input::IsKeyPressed(KS_KEY_SPACE))
			KS_EDITOR_TRACE("SPACE KEY PRESSED");
	}
};


class EditorApp : public Kaimos::Application
{
public:

	EditorApp()
	{
		PushLayer(new LayerTest());
		PushOverlay(new Kaimos::ImGuiLayer());
	}

	~EditorApp() {}
};


Kaimos::Application* Kaimos::CreateApplication()
{
	return new EditorApp();
}