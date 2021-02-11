// --- Entry Point & Kaimos Header---
#include <Kaimos.h>
#include <Core/EntryPoint.h>

// --- Other Includes ---
#include <ImGui/imgui.h>

// --- Editor Includes ---
#include "EditorLayer.h"


namespace Kaimos {

	class EditorApp : public Application
	{
	public:

		EditorApp() : Application("Kaimos Editor")
		{
			PushLayer(new EditorLayer());
		}

		~EditorApp()
		{
		}
	};

	Application* CreateApplication()
	{
		return new EditorApp();
	}
}
