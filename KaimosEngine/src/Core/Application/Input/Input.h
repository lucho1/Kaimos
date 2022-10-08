#ifndef _INPUT_H_
#define _INPUT_H_

#include "KaimosInputCodes.h"
#include <glm/glm.hpp>

namespace Kaimos {

	class Input
	{
		friend class Application;
	public:

		// --- Keyboard Methods ---
		static bool IsKeyPressed(const KEY_CODE key);	// For key pressed down some time
		static bool IsKeyDown(const KEY_CODE key);		// For key pressed and counted only once
		static bool IsKeyUp(const KEY_CODE key);

		// --- Mouse Methods ---
		static bool IsMouseButtonPressed(const MOUSE_CODE button);
		static bool IsMouseButtonDown(const MOUSE_CODE button);
		static bool IsMouseButtonUp(const MOUSE_CODE button);

		// --- Mouse Getters ---
		static glm::vec2 GetMousePos();
		static float GetMouseX();
		static float GetMouseY();
		
		// --- Public Class Methods ---
		static KEY_CODE GetCrossKeyboardKey(const KEY_CODE key);

	protected:

		// --- Protected Class Methods ---
		static bool GetKey(const KEY_CODE key);
		static bool GetMouseButton(const MOUSE_CODE button);

	private:
		
		// --- Private Class Methods ---
		static void OnUpdate();
	};
}

#endif //_INPUT_H_
