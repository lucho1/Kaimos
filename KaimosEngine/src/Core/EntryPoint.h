#ifndef	_ENTRYPOINT_H_
#define _ENTRYPOINT_H_


#ifdef KS_PLATFORM_WINDOWS


extern Kaimos::Application* Kaimos::CreateApplication();

int main(int argc, char** argv)
{
	std::cout << "Kaimos Engine Started" << std::endl;
	Kaimos::Log::Init();

	KS_ENGINE_INFO("Started Engine Logger Successfully");
	KS_ENGINE_TRACE("-- Engine Logger Test --");
	KS_ENGINE_WARN("Warn Test Message");
	KS_ENGINE_ERROR("Error Test Message");
	KS_ENGINE_CRITICAL("Fatal Test Message");

	KS_EDITOR_INFO("Started Editor Logger Successfully");
	KS_EDITOR_TRACE("-- Editor Logger Test --");
	KS_EDITOR_WARN("Warn Test Message");
	KS_EDITOR_ERROR("Error Test Message");
	KS_EDITOR_CRITICAL("Fatal Test Message");

	int a = 5;
	KS_ENGINE_INFO("Numerical Test: Var = {0}", a);

	Kaimos::Application* app = Kaimos::CreateApplication();
	app->Run();
	delete app;

	return 0;
}


#endif // KS_PLATFORM_WINDOWS
#endif // _ENTRYPOINT_H_