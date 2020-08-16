#ifndef	_ENTRYPOINT_H_
#define _ENTRYPOINT_H_


#ifdef KS_PLATFORM_WINDOWS


extern Kaimos::Application* Kaimos::CreateApplication();

int main(int argc, char** argv)
{
	std::cout << "Kaimos Engine Started" << std::endl;
	Kaimos::Application* app = Kaimos::CreateApplication();
	app->Run();
	delete app;

	return 0;
}


#endif //KS_PLATFORM_WINDOWS
#endif //_ENTRYPOINT_H_