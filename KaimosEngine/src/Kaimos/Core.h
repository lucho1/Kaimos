#ifndef _CORE_H_
#define _CORE_H_


//If windows is the current platform (currently not other platform is available)
#ifdef KS_PLATFORM_WINDOWS
	//Build this dll (the current one of Kaimos Engine project, not for other!)
	#ifdef KS_BUILD_DLL
		//When building the DLL (the engine itself), we EXPORT anything after KAIMOS_API
		#define KAIMOS_API __declspec(dllexport)
	#else
		//Otherwise, when not building the DLL (the engine itself), we IMPORT anything after KAIMOS_API
		#define KAIMOS_API __declspec(dllimport)
	#endif
#else
	#error Kaimos Engine only supports Windows currently
#endif


#endif //_CORE_H_