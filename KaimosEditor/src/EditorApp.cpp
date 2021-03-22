// --- Entry Point & Kaimos Header---
#include <Kaimos.h>
#include <Core/EntryPoint.h>

// --- Editor Includes ---
#include "EditorLayer.h"


namespace Kaimos {

	class EditorApp : public Application
	{
	public:

		EditorApp() : Application("Kaimos Editor") { PushLayer(new EditorLayer()); }
		~EditorApp() = default;
	};

	Application* CreateApplication() { return new EditorApp(); }
}
