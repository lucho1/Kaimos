#ifndef _INPUT_H
#define _INPUT_H

#include "Core/Core.h"
#include "KaimosInputCodes.h"

namespace Kaimos {

	class Input
	{
	protected:
		Input() = default;

	public:

		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;

		inline static bool IsKeyPressed(KEYCODE key) { return s_Instance->IsKeyPressedImpl(key); }

		inline static bool IsMouseButtonPressed(MOUSECODE button) { return s_Instance->IsMouseButtonPressedImpl(button); }
		inline static std::pair<float, float> GetMousePos() { return s_Instance->GetMousePosImpl(); }
		inline static float GetMouseX() { return s_Instance->GetMouseXImpl(); }
		inline static float GetMouseY() { return s_Instance->GetMouseYImpl(); }

		static ScopePtr<Input> Create();

	protected:

		// -- To keep platform-abstraction, virtual functions defined per platform (called by public ones above) --
		virtual bool IsKeyPressedImpl(KEYCODE keycode) = 0;

		virtual bool IsMouseButtonPressedImpl(MOUSECODE button) = 0;
		virtual std::pair<float, float> GetMousePosImpl() = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;

	private:

		// Since it's a singleton and lives all the program's lifetime, we can allow to not to call delete, but... not sure... Maybe use a smart pointer?
		//static Input* s_Instance; ... Therefore, yes, using a smart pointer now:
		static ScopePtr<Input> s_Instance;
	};
}

#endif