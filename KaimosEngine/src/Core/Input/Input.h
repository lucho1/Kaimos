#ifndef _INPUT_H
#define _INPUT_H

#include "KaimosInputCodes.h"
#include <glm/glm.hpp>

namespace Kaimos {

	class Input
	{
		friend class Application;
	public:

		// --- Keyboard Functions ---
		static bool IsKeyPressed(const KEY_CODE key);
		static bool IsKeyDown(const KEY_CODE key);
		static bool IsKeyUp(const KEY_CODE key);

		// --- Mouse Functions ---
		static bool IsMouseButtonPressed(const MOUSE_CODE button);
		static bool IsMouseButtonDown(const MOUSE_CODE button);
		static bool IsMouseButtonUp(const MOUSE_CODE button);

		static glm::vec2 GetMousePos();
		static float GetMouseX();
		static float GetMouseY();
		
		// --- Class Public Methods ---
		static KEY_CODE GetCrossKeyboardKey(const KEY_CODE key);

	protected:

		// --- Class Protected Methods ---
		static bool GetKey(const KEY_CODE key);
		static bool GetMouseButton(const MOUSE_CODE button);

	private:
		
		// --- Class Private Methods ---
		static void OnUpdate();
	};
}

#endif
