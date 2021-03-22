#include "kspch.h"
#include "Core/Input/Input.h"

#include "Core/Application.h"
#include <GLFW/glfw3.h>

namespace Kaimos {

	static std::unordered_map<KEY_CODE, bool>			s_KeyMap(false);
	static std::unordered_map<MOUSE_CODE, bool>			s_MouseMap(false);
	static std::unordered_map<JOYSTICK_CODE, bool>		s_JoystickMap(false);
	static std::unordered_map<GAMEPAD_CODE, bool>		s_GamepadMap(false);
	static std::unordered_map<GAMEPADAXIS_CODE, bool>	s_GamepadAxisMap(false);



	// ----------------------- Keyboard Methods -----------------------------------------------------------
	bool Input::IsKeyPressed(const KEY_CODE key)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int state = glfwGetKey(window, static_cast<int32_t>(Input::GetCrossKeyboardKey(key)));

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsKeyDown(const KEY_CODE key)
	{
		return GetKey(key) && !s_KeyMap[key];
	}
	
	bool Input::IsKeyUp(const KEY_CODE key)
	{
		return !GetKey(key) && s_KeyMap[key];
	}


	
	// ----------------------- Mouse Methods --------------------------------------------------------------
	bool Input::IsMouseButtonPressed(const MOUSE_CODE button)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int state = glfwGetMouseButton(window, static_cast<int32_t>(button));

		return state == GLFW_PRESS;
	}

	bool Input::IsMouseButtonDown(const MOUSE_CODE button)
	{
		return GetMouseButton(button) && !s_MouseMap[button];
	}

	bool Input::IsMouseButtonUp(const MOUSE_CODE button)
	{
		return !GetMouseButton(button) && s_MouseMap[button];
	}



	// ----------------------- Mouse Getters --------------------------------------------------------------
	glm::vec2 Input::GetMousePos()
	{
		double x = 0.0, y = 0.0;
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		glfwGetCursorPos(window, &x, &y);
		return { (float)x, (float)y };
	}

	float Input::GetMouseX()
	{
		return GetMousePos().x;
	}

	float Input::GetMouseY()
	{
		return GetMousePos().y;
	}



	// ----------------------- Public Class Methods -------------------------------------------------------
	KEY_CODE Input::GetCrossKeyboardKey(const KEY_CODE key)
	{
		const char* key_char = glfwGetKeyName(int(key), glfwGetKeyScancode(int(key)));

		// -- Key is between A and Z --
		if (key >= KEY::A && key <= KEY::Z)
		{
			if (key_char != nullptr)
				return KEY_CODE(int(KEY::A) + (std::toupper(key_char[0]) - 'A'));
		}
		// -- Key is other key (like punctuation keys) --
		else if (key > KEY::SPACE && key < KEY::WORLD2)
		{
			if (key_char != nullptr)
				return KEY_CODE(int(KEY::SPACE) + (std::toupper(key_char[0]) - ' '));
		}
		
		// -- Other keys are independent of the keyboard (like shift, control, space...) as well as digits, treated as they are regardless of keyboard --
		return key;
	}



	// ----------------------- Protected Class Methods ----------------------------------------------------
	bool Input::GetKey(const KEY_CODE key)
	{
		return GLFW_PRESS == glfwGetKey(static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow()), static_cast<int32_t>(key));
	}

	bool Input::GetMouseButton(const MOUSE_CODE button)
	{
		return GLFW_PRESS == glfwGetMouseButton(static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow()), static_cast<int32_t>(button));
	}


	
	// ----------------------- Private Class Methods ------------------------------------------------------
	void Input::OnUpdate()
	{
		for (KEY_CODE key : s_KeysVec)
			s_KeyMap[key] = GetKey(key);

		for (MOUSE_CODE btn : s_MouseButtonsVec)
			s_MouseMap[btn] = GetMouseButton(btn);
	}
}
