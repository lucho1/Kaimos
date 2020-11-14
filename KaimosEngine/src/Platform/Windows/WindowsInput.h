#ifndef _WINDOWSINPUT_H
#define _WINDOWSINPUT_H

#include "Core/Input/Input.h"

namespace Kaimos {

	class WindowsInput : public Input
	{
	protected:

		virtual bool IsKeyPressedImpl(KEYCODE key) override;

		virtual bool IsMouseButtonPressedImpl(MOUSECODE button) override;
		virtual std::pair<float, float> GetMousePosImpl() override;
		virtual float GetMouseXImpl() override;
		virtual float GetMouseYImpl() override;
	};

}

#endif