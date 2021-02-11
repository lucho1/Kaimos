#ifndef _INPUT_H
#define _INPUT_H

#include "KaimosInputCodes.h"
#include <glm/glm.hpp>

namespace Kaimos {

	class Input
	{
	public:

		static bool IsKeyPressed(KEY_CODE key);

		static bool IsMouseButtonPressed(MOUSE_CODE button);
		static glm::vec2 GetMousePos();
		static float GetMouseX();
		static float GetMouseY();
	};
}

#endif
