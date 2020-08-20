#ifndef _INPUT_H
#define _INPUT_H

#include "Core/Core.h"

namespace Kaimos {

	class KAIMOS_API Input
	{
	public:

		inline static bool IsKeyPressed(uint keycode) { return s_Instance->IsKeyPressedImpl(keycode); }

		inline static bool IsMouseButtonPressed(uint button) { return s_Instance->IsMouseButtonPressedImpl(button); }
		inline static std::pair<float, float> GetMousePos() { return s_Instance->GetMousePosImpl(); }
		inline static float GetMouseX() { return s_Instance->GetMouseXImpl(); }
		inline static float GetMouseY() { return s_Instance->GetMouseYImpl(); }

	protected:

		// -- To keep platform-abstraction, virtual functions defined per platform (called by public ones above) --
		virtual bool IsKeyPressedImpl(uint keycode) = 0;
		
		virtual bool IsMouseButtonPressedImpl(uint button) = 0;
		virtual std::pair<float, float> GetMousePosImpl() = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;

	private:

		static Input* s_Instance; // Since it's a singleton and lives all the program's lifetime, we can allow to not to call delete, but... not sure... Maybe use a smart pointer?
	};
}

#endif