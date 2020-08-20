#include <Kaimos.h>

class LayerTest : public Kaimos::Layer
{
public:

	LayerTest() : Layer("LayerTest") {}

	void OnUpdate() override { KS_EDITOR_INFO("LayerTest Update"); }
	void OnEvent(Kaimos::Event& ev) override { KS_EDITOR_TRACE("LayerTest Event: {0}", ev); }
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