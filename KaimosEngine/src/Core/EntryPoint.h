#ifndef	_ENTRYPOINT_H_
#define _ENTRYPOINT_H_


extern "C" {
	// http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;

	// http://developer.amd.com/community/blog/2015/10/02/amd-enduro-system-for-developers/
	// or (if the 1st doesn't works): https://gpuopen.com/amdpowerxpressrequesthighperformance/ or https://community.amd.com/thread/169965
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}


#ifdef KS_PLATFORM_WINDOWS

#if KS_DEBUG
	#define SESSION_NAME(name) "<KS_DEBUG> - "##name
	#define SESSION_FILENAME(name) "../Profiling/KaimosDebug_Profile"##name
#elif KS_RELEASE
	#define SESSION_NAME(name) "<KS_RELEASE> - "##name
	#define SESSION_FILENAME(name) "../Profiling/KaimosRelease_Profile"##name
#else
	#define SESSION_NAME(name) "<WRONG_CONFIG> - "##name
	#define SESSION_FILENAME(name) "../Profiling/WrongConfig_Profile"##name
#endif

extern Kaimos::Application* Kaimos::CreateApplication();

int main(int argc, char** argv)
{
	Kaimos::Log::Init();
	KS_ENGINE_INFO("--- Kaimos Engine Started ---");
	KS_ENGINE_INFO("	Initialized Logger");


	KS_ENGINE_INFO("--- Creating Kaimos Application ---");
	KS_PROFILE_BEGIN_SESSION(SESSION_NAME("Startup"), SESSION_FILENAME("Startup.json"));
	Kaimos::Application* app = Kaimos::CreateApplication();
	KS_PROFILE_END_SESSION();

	KS_ENGINE_INFO("--- Running Kaimos Application ---");
	KS_PROFILE_BEGIN_SESSION(SESSION_NAME("Runtime"), SESSION_FILENAME("Runtime.json"));
	app->Run();
	KS_PROFILE_END_SESSION();

	KS_ENGINE_INFO("--- Shutting Down Kaimos Application ---");
	KS_PROFILE_BEGIN_SESSION(SESSION_NAME("Shutdown"), SESSION_FILENAME("Shutdown.json"));
	delete app;
	KS_PROFILE_END_SESSION();

	return 0;
}


#endif // KS_PLATFORM_WINDOWS
#endif // _ENTRYPOINT_H_