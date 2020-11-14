#ifndef _KAIMOSINPUTCODES_H_
#define _KAIMOSINPUTCODES_H_

// ------------ Kaimos Engine redefinition for Input Codes, from glfw3.h ------------
namespace Kaimos
{
	typedef enum class KEYCODE : uint16_t
	{
		Space = 32,
		Apostrophe = 39,	/* ' */
		Comma = 44,			/* , */
		Minus = 45,			/* - */
		Period = 46,		/* . */
		Slash = 47,			/* / */

		D0 = 48,			/* 0 */
		D1 = 49,			/* 1 */
		D2 = 50,			/* 2 */
		D3 = 51,			/* 3 */
		D4 = 52,			/* 4 */
		D5 = 53,			/* 5 */
		D6 = 54,			/* 6 */
		D7 = 55,			/* 7 */
		D8 = 56,			/* 8 */
		D9 = 57,			/* 9 */

		Semicolon = 59,		/* ; */
		Equal = 61,			/* = */

		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,

		LeftBracket = 91,	/* [ */
		Backslash = 92,		/* \ */
		RightBracket = 93,	/* ] */
		GraveAccent = 96,	/* ` */

		World1 = 161,		/* non-US #1 */
		World2 = 162,		/* non-US #2 */

		/* Function keys */
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,

		/* Keypad */
		KP0 = 320,
		KP1 = 321,
		KP2 = 322,
		KP3 = 323,
		KP4 = 324,
		KP5 = 325,
		KP6 = 326,
		KP7 = 327,
		KP8 = 328,
		KP9 = 329,
		KPDecimal = 330,
		KPDivide = 331,
		KPMultiply = 332,
		KPSubtract = 333,
		KPAdd = 334,
		KPEnter = 335,
		KPEqual = 336,

		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348
	} Key;

	inline std::ostream& operator<<(std::ostream& os, KEYCODE keyCode)
	{
		os << static_cast<int32_t>(keyCode);
		return os;
	}
}


// ------------ KEYBOARD CODES ------------
/* The unknown key */
#define KS_KEY_UNKNOWN            -1

/* -- Printable keys -- */
#define KS_KEY_SPACE			::Kaimos::Key::Space
#define KS_KEY_APOSTROPHE		::Kaimos::Key::Apostrophe	/* ' */
#define KS_KEY_COMMA			::Kaimos::Key::Comma		/* , */
#define KS_KEY_MINUS			::Kaimos::Key::Minus		/* - */
#define KS_KEY_PERIOD			::Kaimos::Key::Period		/* . */
#define KS_KEY_SLASH			::Kaimos::Key::Slash		/* / */
#define KS_KEY_0				::Kaimos::Key::D0
#define KS_KEY_1				::Kaimos::Key::D1
#define KS_KEY_2				::Kaimos::Key::D2
#define KS_KEY_3				::Kaimos::Key::D3
#define KS_KEY_4				::Kaimos::Key::D4
#define KS_KEY_5				::Kaimos::Key::D5
#define KS_KEY_6				::Kaimos::Key::D6
#define KS_KEY_7				::Kaimos::Key::D7
#define KS_KEY_8				::Kaimos::Key::D8
#define KS_KEY_9				::Kaimos::Key::D9
#define KS_KEY_SEMICOLON		::Kaimos::Key::Semicolon	/* ; */
#define KS_KEY_EQUAL			::Kaimos::Key::Equal		/* = */
#define KS_KEY_A				::Kaimos::Key::A
#define KS_KEY_B				::Kaimos::Key::B
#define KS_KEY_C				::Kaimos::Key::C
#define KS_KEY_D				::Kaimos::Key::D
#define KS_KEY_E				::Kaimos::Key::E
#define KS_KEY_F				::Kaimos::Key::F
#define KS_KEY_G				::Kaimos::Key::G
#define KS_KEY_H				::Kaimos::Key::H
#define KS_KEY_I				::Kaimos::Key::I
#define KS_KEY_J				::Kaimos::Key::J
#define KS_KEY_K				::Kaimos::Key::K
#define KS_KEY_L				::Kaimos::Key::L
#define KS_KEY_M				::Kaimos::Key::M
#define KS_KEY_N				::Kaimos::Key::N
#define KS_KEY_O				::Kaimos::Key::O
#define KS_KEY_P				::Kaimos::Key::P
#define KS_KEY_Q				::Kaimos::Key::Q
#define KS_KEY_R				::Kaimos::Key::R
#define KS_KEY_S				::Kaimos::Key::S
#define KS_KEY_T				::Kaimos::Key::T
#define KS_KEY_U				::Kaimos::Key::U
#define KS_KEY_V				::Kaimos::Key::V
#define KS_KEY_W				::Kaimos::Key::W
#define KS_KEY_X				::Kaimos::Key::X
#define KS_KEY_Y				::Kaimos::Key::Y
#define KS_KEY_Z				::Kaimos::Key::Z
#define KS_KEY_LEFT_BRACKET		::Kaimos::Key::LeftBracket	/* [ */
#define KS_KEY_BACKSLASH		::Kaimos::Key::Backslash	/* \ */
#define KS_KEY_RIGHT_BRACKET	::Kaimos::Key::RightBracket	/* ] */
#define KS_KEY_GRAVE_ACCENT		::Kaimos::Key::GraveAccent	/* ` */
#define KS_KEY_WORLD_1			::Kaimos::Key::World1		/* non-US #1 */
#define KS_KEY_WORLD_2			::Kaimos::Key::World2		/* non-US #2 */

/* -- Function keys -- */
#define KS_KEY_ESCAPE			::Kaimos::Key::Escape
#define KS_KEY_ENTER			::Kaimos::Key::Enter
#define KS_KEY_TAB				::Kaimos::Key::Tab
#define KS_KEY_BACKSPACE		::Kaimos::Key::Backspace
#define KS_KEY_INSERT			::Kaimos::Key::Insert
#define KS_KEY_DELETE			::Kaimos::Key::Delete
#define KS_KEY_RIGHT			::Kaimos::Key::Right
#define KS_KEY_LEFT				::Kaimos::Key::Left
#define KS_KEY_DOWN				::Kaimos::Key::Down
#define KS_KEY_UP				::Kaimos::Key::Up
#define KS_KEY_PAGE_UP			::Kaimos::Key::PageUp
#define KS_KEY_PAGE_DOWN		::Kaimos::Key::PageDown
#define KS_KEY_HOME				::Kaimos::Key::Home
#define KS_KEY_END				::Kaimos::Key::End
#define KS_KEY_CAPS_LOCK		::Kaimos::Key::CapsLock
#define KS_KEY_SCROLL_LOCK		::Kaimos::Key::ScrollLock
#define KS_KEY_NUM_LOCK			::Kaimos::Key::NumLock
#define KS_KEY_PRINT_SCREEN		::Kaimos::Key::PrintScreen
#define KS_KEY_PAUSE			::Kaimos::Key::Pause
#define KS_KEY_F1				::Kaimos::Key::F1
#define KS_KEY_F2				::Kaimos::Key::F2
#define KS_KEY_F3				::Kaimos::Key::F3
#define KS_KEY_F4				::Kaimos::Key::F4
#define KS_KEY_F5				::Kaimos::Key::F5
#define KS_KEY_F6				::Kaimos::Key::F6
#define KS_KEY_F7				::Kaimos::Key::F7
#define KS_KEY_F8				::Kaimos::Key::F8
#define KS_KEY_F9				::Kaimos::Key::F9
#define KS_KEY_F10				::Kaimos::Key::F10
#define KS_KEY_F11				::Kaimos::Key::F11
#define KS_KEY_F12				::Kaimos::Key::F12
#define KS_KEY_F13				::Kaimos::Key::F13
#define KS_KEY_F14				::Kaimos::Key::F14
#define KS_KEY_F15				::Kaimos::Key::F15
#define KS_KEY_F16				::Kaimos::Key::F16
#define KS_KEY_F17				::Kaimos::Key::F17
#define KS_KEY_F18				::Kaimos::Key::F18
#define KS_KEY_F19				::Kaimos::Key::F19
#define KS_KEY_F20				::Kaimos::Key::F20
#define KS_KEY_F21				::Kaimos::Key::F21
#define KS_KEY_F22				::Kaimos::Key::F22
#define KS_KEY_F23				::Kaimos::Key::F23
#define KS_KEY_F24				::Kaimos::Key::F24
#define KS_KEY_F25				::Kaimos::Key::F25

/* Keypad */
#define KS_KEY_KP_0				::Kaimos::Key::KP0
#define KS_KEY_KP_1				::Kaimos::Key::KP1
#define KS_KEY_KP_2				::Kaimos::Key::KP2
#define KS_KEY_KP_3				::Kaimos::Key::KP3
#define KS_KEY_KP_4				::Kaimos::Key::KP4
#define KS_KEY_KP_5				::Kaimos::Key::KP5
#define KS_KEY_KP_6				::Kaimos::Key::KP6
#define KS_KEY_KP_7				::Kaimos::Key::KP7
#define KS_KEY_KP_8				::Kaimos::Key::KP8
#define KS_KEY_KP_9				::Kaimos::Key::KP9
#define KS_KEY_KP_DECIMAL		::Kaimos::Key::KPDecimal
#define KS_KEY_KP_DIVIDE		::Kaimos::Key::KPDivide
#define KS_KEY_KP_MULTIPLY		::Kaimos::Key::KPMultiply
#define KS_KEY_KP_SUBTRACT		::Kaimos::Key::KPSubtract
#define KS_KEY_KP_ADD			::Kaimos::Key::KPAdd
#define KS_KEY_KP_ENTER			::Kaimos::Key::KPEnter
#define KS_KEY_KP_EQUAL			::Kaimos::Key::KPEqual

#define KS_KEY_LEFT_SHIFT		::Kaimos::Key::LeftShift
#define KS_KEY_LEFT_CONTROL		::Kaimos::Key::LeftControl
#define KS_KEY_LEFT_ALT			::Kaimos::Key::LeftAlt
#define KS_KEY_LEFT_SUPER		::Kaimos::Key::LeftSuper
#define KS_KEY_RIGHT_SHIFT		::Kaimos::Key::RightShift
#define KS_KEY_RIGHT_CONTROL	::Kaimos::Key::RightControl
#define KS_KEY_RIGHT_ALT		::Kaimos::Key::RightAlt
#define KS_KEY_RIGHT_SUPER		::Kaimos::Key::RightSuper
#define KS_KEY_MENU				::Kaimos::Key::Menu

#define KS_KEY_LAST				KS_KEY_MENU

/* -- Modifier keys -- */
// MOD = Modifier key flags
// @brief If this bit is set one or more Shift keys were held down.
#define KS_MOD_SHIFT			0x0001
// @brief If this bit is set one or more Control keys were held down.
#define KS_MOD_CONTROL			0x0002
// @brief If this bit is set one or more Alt keys were held down.
#define KS_MOD_ALT				0x0004
//@brief If this bit is set one or more Super keys were held down.
#define KS_MOD_SUPER			0x0008
//@brief If this bit is set the Caps Lock key is enabled.
#define KS_MOD_CAPS_LOCK		0x0010
//@brief If this bit is set the Num Lock key is enabled.
#define KS_MOD_NUM_LOCK			0x0020



// ------------ MOUSE CODES ------------
/* -- Printable keys -- */
namespace Kaimos
{
	typedef enum class MOUSECODE : uint16_t
	{
		Button0 = 0,
		Button1 = 1,
		Button2 = 2,
		Button3 = 3,
		Button4 = 4,
		Button5 = 5,
		Button6 = 6,
		Button7 = 7,

		ButtonLeft = Button0,
		ButtonRight = Button1,
		ButtonMiddle = Button2,
		ButtonLast = Button7
	} Mouse;

	inline std::ostream& operator<<(std::ostream& os, MOUSECODE mouseCode)
	{
		os << static_cast<int32_t>(mouseCode);
		return os;
	}
}


#define KS_MOUSE_BUTTON_0		::Kaimos::Mouse::Button0
#define KS_MOUSE_BUTTON_1		::Kaimos::Mouse::Button1
#define KS_MOUSE_BUTTON_2		::Kaimos::Mouse::Button2
#define KS_MOUSE_BUTTON_3		::Kaimos::Mouse::Button3
#define KS_MOUSE_BUTTON_4		::Kaimos::Mouse::Button4
#define KS_MOUSE_BUTTON_5		::Kaimos::Mouse::Button5
#define KS_MOUSE_BUTTON_6		::Kaimos::Mouse::Button6
#define KS_MOUSE_BUTTON_7		::Kaimos::Mouse::Button7
#define KS_MOUSE_BUTTON_LAST	::Kaimos::Mouse::ButtonLast
#define KS_MOUSE_BUTTON_LEFT	::Kaimos::Mouse::ButtonLeft
#define KS_MOUSE_BUTTON_RIGHT	::Kaimos::Mouse::ButtonRight
#define KS_MOUSE_BUTTON_MIDDLE	::Kaimos::Mouse::ButtonMiddle



// ------------ JOYSTICK CODES ------------
namespace Kaimos
{
	typedef enum class JOYSTICKCODE : uint16_t
	{
		Joystick0 = 0,
		Joystick1 = 1,
		Joystick2 = 2,
		Joystick3 = 3,
		Joystick4 = 4,
		Joystick5 = 5,
		Joystick6 = 6,
		Joystick7 = 7,
		Joystick8 = 8,
		Joystick9 = 9,
		Joystick10 = 10,
		Joystick11 = 11,
		Joystick12 = 12,
		Joystick13 = 13,
		Joystick14 = 14,
		Joystick15 = 15
	} Joystick;

	inline std::ostream& operator<<(std::ostream& os, JOYSTICKCODE joystickCode)
	{
		os << static_cast<int32_t>(joystickCode);
		return os;
	}
}

#define KS_JOYSTICK_1			::Kaimos::Joystick::Joystick0
#define KS_JOYSTICK_2			::Kaimos::Joystick::Joystick1
#define KS_JOYSTICK_3			::Kaimos::Joystick::Joystick2
#define KS_JOYSTICK_4			::Kaimos::Joystick::Joystick3
#define KS_JOYSTICK_5			::Kaimos::Joystick::Joystick4
#define KS_JOYSTICK_6			::Kaimos::Joystick::Joystick5
#define KS_JOYSTICK_7			::Kaimos::Joystick::Joystick6
#define KS_JOYSTICK_8			::Kaimos::Joystick::Joystick7
#define KS_JOYSTICK_9			::Kaimos::Joystick::Joystick8
#define KS_JOYSTICK_10			::Kaimos::Joystick::Joystick9
#define KS_JOYSTICK_11			::Kaimos::Joystick::Joystick10
#define KS_JOYSTICK_12			::Kaimos::Joystick::Joystick11
#define KS_JOYSTICK_13			::Kaimos::Joystick::Joystick12
#define KS_JOYSTICK_14			::Kaimos::Joystick::Joystick13
#define KS_JOYSTICK_15			::Kaimos::Joystick::Joystick14
#define KS_JOYSTICK_16			::Kaimos::Joystick::Joystick15
#define KS_JOYSTICK_LAST		KS_JOYSTICK_16



// ------------ GAMEPAD CODES ------------
namespace Kaimos
{
	typedef enum class GAMEPADCODE : uint16_t
	{
		GamepadBtnA = 0,
		GamepadBtnB = 1,
		GamepadBtnX = 2,
		GamepadBtnY = 3,
		GamepadBtnLBump = 4,
		GamepadBtnRBump = 5,
		GamepadBtnBack = 6,
		GamepadBtnStart = 7,
		GamepadBtnGuide = 8,
		GamepadBtnLThumb = 9,
		GamepadBtnRThumb = 10,
		GamepadBtnDP_Up = 11,
		GamepadBtnDP_Right = 12,
		GamepadBtnDP_Down = 13,
		GamepadBtnDP_Left = 14
	} Gamepad;

	inline std::ostream& operator<<(std::ostream& os, GAMEPADCODE gamepadCode)
	{
		os << static_cast<int32_t>(gamepadCode);
		return os;
	}
}

#define KS_GAMEPAD_BUTTON_A				::Kaimos::Gamepad::GamepadBtnA
#define KS_GAMEPAD_BUTTON_B				::Kaimos::Gamepad::GamepadBtnB
#define KS_GAMEPAD_BUTTON_X				::Kaimos::Gamepad::GamepadBtnX
#define KS_GAMEPAD_BUTTON_Y				::Kaimos::Gamepad::GamepadBtnY
#define KS_GAMEPAD_BUTTON_LEFT_BUMPER	::Kaimos::Gamepad::GamepadBtnLBump
#define KS_GAMEPAD_BUTTON_RIGHT_BUMPER	::Kaimos::Gamepad::GamepadBtnRBump
#define KS_GAMEPAD_BUTTON_BACK			::Kaimos::Gamepad::GamepadBtnBack
#define KS_GAMEPAD_BUTTON_START			::Kaimos::Gamepad::GamepadBtnStart
#define KS_GAMEPAD_BUTTON_GUIDE			::Kaimos::Gamepad::GamepadBtnGuide
#define KS_GAMEPAD_BUTTON_LEFT_THUMB	::Kaimos::Gamepad::GamepadBtnLThumb
#define KS_GAMEPAD_BUTTON_RIGHT_THUMB	::Kaimos::Gamepad::GamepadBtnRThumb
#define KS_GAMEPAD_BUTTON_DPAD_UP		::Kaimos::Gamepad::GamepadBtnDP_Up
#define KS_GAMEPAD_BUTTON_DPAD_RIGHT	::Kaimos::Gamepad::GamepadBtnDP_Right
#define KS_GAMEPAD_BUTTON_DPAD_DOWN		::Kaimos::Gamepad::GamepadBtnDP_Down
#define KS_GAMEPAD_BUTTON_DPAD_LEFT		::Kaimos::Gamepad::GamepadBtnDP_Left
#define KS_GAMEPAD_BUTTON_LAST            KS_GAMEPAD_BUTTON_DPAD_LEFT

#define KS_GAMEPAD_BUTTON_CROSS       KS_GAMEPAD_BUTTON_A
#define KS_GAMEPAD_BUTTON_CIRCLE      KS_GAMEPAD_BUTTON_B
#define KS_GAMEPAD_BUTTON_SQUARE      KS_GAMEPAD_BUTTON_X
#define KS_GAMEPAD_BUTTON_TRIANGLE    KS_GAMEPAD_BUTTON_Y

/* -- Gamepad Axis -- */
namespace Kaimos
{
	typedef enum class GAMEPADAXISCODE : uint16_t
	{
		GamepadAxis_LX = 0,
		GamepadAxis_LY = 1,
		GamepadAxis_RX = 2,
		GamepadAxis_RY = 3,
		GamepadAxis_LTrigger = 4,
		GamepadAxis_RTrigger = 5,
	} GamepadAxis;

	inline std::ostream& operator<<(std::ostream& os, GAMEPADAXISCODE gamepadCode)
	{
		os << static_cast<int32_t>(gamepadCode);
		return os;
	}
}

#define KS_GAMEPAD_AXIS_LEFT_X			::Kaimos::GamepadAxis::GamepadAxis_LX
#define KS_GAMEPAD_AXIS_LEFT_Y			::Kaimos::GamepadAxis::GamepadAxis_LY
#define KS_GAMEPAD_AXIS_RIGHT_X			::Kaimos::GamepadAxis::GamepadAxis_RX
#define KS_GAMEPAD_AXIS_RIGHT_Y			::Kaimos::GamepadAxis::GamepadAxis_RY
#define KS_GAMEPAD_AXIS_LEFT_TRIGGER	::Kaimos::GamepadAxis::GamepadAxis_LTrigger
#define KS_GAMEPAD_AXIS_RIGHT_TRIGGER	::Kaimos::GamepadAxis::GamepadAxis_RTrigger
#define KS_GAMEPAD_AXIS_LAST			KS_GAMEPAD_AXIS_RIGHT_TRIGGER



// ---------------------------------------
// ---------------------------------------
#endif // _KAIMOSINPUTCODES_H_