#include "kspch.h"
#include "Core/Input/Input.h"

#include "Core/Application.h"
#include <GLFW/glfw3.h>

namespace Kaimos {

	bool Input::IsKeyPressed(const KEY_CODE key)
	{
		KEY_CODE k = Input::GetCrossKeyboardKey(key);
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

		int state = glfwGetKey(window, static_cast<int32_t>(k));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsMouseButtonPressed(const MOUSE_CODE button)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

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

	KEY_CODE Input::GetCrossKeyboardKey(KEY_CODE key)
	{
		const char* key_char = glfwGetKeyName(int(key), glfwGetKeyScancode(int(key)));

		// Key is between A and Z
		if (key >= KEY::A && key <= KEY::Z)
		{
			if (key_char != nullptr)
				return KEY_CODE(int(KEY::A) + (std::toupper(key_char[0]) - 'A'));
		}
		// Key is other key (like punctuation keys)
		else if (key > KEY::SPACE && key < KEY::WORLD2)
		{
			if (key_char != nullptr)
				return KEY_CODE(int(KEY::SPACE) + (std::toupper(key_char[0]) - ' '));
		}
		
		// Other keys are independent of the keyboard (like shift, control, space...) as well as digits, treated as they are regardless of keyboard
		return key;
	}
}
