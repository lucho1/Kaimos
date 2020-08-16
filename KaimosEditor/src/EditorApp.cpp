#include <Kaimos.h>

class EditorApp : public Kaimos::Application
{
public:
	EditorApp() {}
	~EditorApp() {}
};

Kaimos::Application* Kaimos::CreateApplication()
{
	return new EditorApp();
}