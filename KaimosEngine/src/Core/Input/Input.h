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
		static bool IsKeyPressed(const KEY_CODE key);
		inline static bool IsKeyDown(const KEY_CODE key);
		inline static bool IsKeyUp(const KEY_CODE key);

		// --- Mouse Methods ---
		static bool IsMouseButtonPressed(const MOUSE_CODE button);
		inline static bool IsMouseButtonDown(const MOUSE_CODE button);
		inline static bool IsMouseButtonUp(const MOUSE_CODE button);

		// --- Mouse Getters ---
		static glm::vec2 GetMousePos();
		inline static float GetMouseX();
		inline static float GetMouseY();
		
		// --- Public Class Methods ---
		static KEY_CODE GetCrossKeyboardKey(const KEY_CODE key);

	protected:

		// --- Protected Class Methods ---
		inline static bool GetKey(const KEY_CODE key);
		inline static bool GetMouseButton(const MOUSE_CODE button);

	private:
		
		// --- Private Class Methods ---
		static void OnUpdate();
	};
}

#endif //_INPUT_H_
