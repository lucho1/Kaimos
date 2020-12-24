#ifndef _INPUT_H
#define _INPUT_H

#include "Core/Core.h"
#include "KaimosInputCodes.h"

namespace Kaimos {

	class Input
	{
	public:

		static bool IsKeyPressed(KEYCODE key);

		static bool IsMouseButtonPressed(MOUSECODE button);
		static std::pair<float, float> GetMousePos();
		static float GetMouseX();
		static float GetMouseY();
	};
}

#endif