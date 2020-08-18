#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "Core.h"

namespace Kaimos {

	class KAIMOS_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};


	// To be defined in Client (Editor)
	Application* CreateApplication();
}

#endif