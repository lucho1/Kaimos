#ifndef _INPUT_H
#define _INPUT_H

#include "KaimosInputCodes.h"
#include <glm/glm.hpp>

namespace Kaimos {

	class Input
	{
	public:

		static bool IsKeyPressed(KEYCODE key);

		static bool IsMouseButtonPressed(MOUSECODE button);
		static glm::vec2 GetMousePos();
		static float GetMouseX();
		static float GetMouseY();
	};
}

#endif
