#ifndef _KAIMOSINPUTCODES_H_
#define _KAIMOSINPUTCODES_H_

// ------------ Kaimos Engine redefinition for Input Codes, from glfw3.h ------------
namespace Kaimos
{
	using KEY_CODE = uint16_t;
	namespace KEY
	{
		enum KEY_CODE
		{
			SPACE = 32,
			APOSTROPHE = 39,	/* ' */
			COMMA = 44,			/* , */
			MINUS = 45,			/* - */
			PERIOD = 46,		/* . */
			SLASH = 47,			/* / */

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

			SEMICOLON = 59,		/* ; */
			EQUAL = 61,			/* = */

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

			LEFT_BRACKET = 91,	/* [ */
			BACK_SLASH = 92,		/* \ */
			RIGHT_BRACKET = 93,	/* ] */
			GRAVE_ACCENT = 96,	/* ` */

			WORLD1 = 161,		/* non-US #1 */
			WORLD2 = 162,		/* non-US #2 */

			/* Function keys */
			ESCAPE = 256,
			ENTER = 257,
			TAB = 258,
			BACKSPACE = 259,
			INSERT = 260,
			DEL = 261,
			RIGHT = 262,
			LEFT = 263,
			DOWN = 264,
			UP = 265,
			PAGE_UP = 266,
			PAGE_DOWN = 267,
			HOME = 268,
			END = 269,
			CAPSLOCK = 280,
			SCROLL_LOCK = 281,
			NUM_LOCK = 282,
			PRINT_SCREEN = 283,
			PAUSE = 284,
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
			KP_DECIMAL = 330,
			KP_DIVIDE = 331,
			KP_MULTIPLY = 332,
			KP_SUBTRACT = 333,
			KP_ADD = 334,
			KP_ENTER = 335,
			KP_EQUAL = 336,

			LEFT_SHIFT = 340,
			LEFT_CONTROL = 341,
			LEFT_ALT = 342,
			LEFT_SUPER = 343,
			RIGHT_SHIFT = 344,
			RIGHT_CONTROL = 345,
			RIGHT_ALT = 346,
			RIGHT_SUPER = 347,
			MENU = 348
		};
	}
}


/* -- Modifier keys -- */
// MOD = Modifier key flags
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
	using MOUSE_CODE = uint16_t;
	namespace MOUSE
	{
		enum MOUSE_CODE
		{
			BUTTON0 = 0,
			BUTTON1 = 1,
			BUTTON2 = 2,
			BUTTON3 = 3,
			BUTTON4 = 4,
			BUTTON5 = 5,
			BUTTON6 = 6,
			BUTTON7 = 7,

			BUTTON_LEFT = BUTTON0,
			BUTTON_RIGHT = BUTTON1,
			BUTTON_MIDDLE = BUTTON2,
			BUTTON_LAST = BUTTON7
		};
	}
}



// ------------ JOYSTICK CODES ------------
namespace Kaimos
{
	using JOYSTICK_CODE = uint16_t;
	namespace JOYSTICK
	{
		enum JOYSTICK_CODE
		{
			JOYSTICK0 = 0,
			JOYSTICK1 = 1,
			JOYSTICK2 = 2,
			JOYSTICK3 = 3,
			JOYSTICK4 = 4,
			JOYSTICK5 = 5,
			JOYSTICK6 = 6,
			JOYSTICK7 = 7,
			JOYSTICK8 = 8,
			JOYSTICK9 = 9,
			JOYSTICK10 = 10,
			JOYSTICK11 = 11,
			JOYSTICK12 = 12,
			JOYSTICK13 = 13,
			JOYSTICK14 = 14,
			JOYSTICK15 = 15
		};
	}
}



// ------------ GAMEPAD CODES ------------
namespace Kaimos
{
	using GAMEPAD_CODE = uint16_t;
	namespace GAMEPAD
	{
		enum GAMEPAD_CODE
		{
			GAMEPAD_BTN_A = 0,
			GAMEPAD_BTN_B = 1,
			GAMEPAD_BTN_X = 2,
			GAMEPAD_BTN_Y = 3,
			GAMEPAD_BTN_LBUMP = 4,
			GAMEPAD_BTN_RBUMP = 5,
			GAMEPAD_BTN_BACK = 6,
			GAMEPAD_BTN_START = 7,
			GAMEPAD_BTN_GUIDE = 8,
			GAMEPAD_BTN_LTHUMB = 9,
			GAMEPAD_BTN_RTHUMB = 10,
			GAMEPAD_BTNDP_UP = 11,
			GAMEPAD_BTNDP_RIGHT = 12,
			GAMEPAD_BTNDP_DOWN = 13,
			GAMEPAD_BTNDP_LEFT = 14,
				   
			GAMEPAD_BTN_LAST = GAMEPAD_BTNDP_LEFT,
			GAMEPAD_BTN_CROSS = GAMEPAD_BTN_A,
			GAMEPAD_BTN_CIRCLE = GAMEPAD_BTN_B,
			GAMEPAD_BTN_SQUARE = GAMEPAD_BTN_X,
			GAMEPAD_BTN_TRAINGLE = GAMEPAD_BTN_Y
		};
	}
}



/* -- Gamepad Axis -- */
namespace Kaimos
{
	using GAMEPADAXIS_CODE = uint16_t;
	namespace GAMEPADAXIS
	{
		enum GAMEPADAXIS_CODE
		{
			GAMEPAD_AXIS_LX = 0,
			GAMEPAD_AXIS_LY = 1,
			GAMEPAD_AXIS_RX = 2,
			GAMEPAD_AXIS_RY = 3,
			GAMEPAD_AXIS_LTRIGGER = 4,
			GAMEPAD_AXIS_RTRIGGER = 5,
			GAMEPAD_AXIS_LAST = GAMEPAD_AXIS_RTRIGGER
		};
	}
}



// ---------------------------------------
// ---------------------------------------
// ------------ INPUT VECTORS ------------
namespace Kaimos {

	// --- Keys ---
	static std::vector<KEY_CODE> s_KeysVec =
	{
		KEY::SPACE,
		KEY::APOSTROPHE,
		KEY::COMMA,
		KEY::MINUS,
		KEY::PERIOD,
		KEY::SLASH,
		KEY::D0,
		KEY::D1,
		KEY::D2,
		KEY::D3,
		KEY::D4,
		KEY::D5,
		KEY::D6,
		KEY::D7,
		KEY::D8,
		KEY::D9,
		KEY::SEMICOLON,
		KEY::EQUAL,
		KEY::A,
		KEY::B,
		KEY::C,
		KEY::D,
		KEY::E,
		KEY::F,
		KEY::G,
		KEY::H,
		KEY::I,
		KEY::J,
		KEY::K,
		KEY::L,
		KEY::M,
		KEY::N,
		KEY::O,
		KEY::P,
		KEY::Q,
		KEY::R,
		KEY::S,
		KEY::T,
		KEY::U,
		KEY::V,
		KEY::W,
		KEY::X,
		KEY::Y,
		KEY::Z,
		KEY::LEFT_BRACKET,
		KEY::BACK_SLASH,
		KEY::RIGHT_BRACKET,
		KEY::GRAVE_ACCENT,
		KEY::WORLD1,
		KEY::WORLD2,
		KEY::ESCAPE,
		KEY::ENTER,
		KEY::TAB,
		KEY::BACKSPACE,
		KEY::INSERT,
		KEY::DEL,
		KEY::RIGHT,
		KEY::LEFT,
		KEY::DOWN,
		KEY::UP,
		KEY::PAGE_UP,
		KEY::PAGE_DOWN,
		KEY::HOME,
		KEY::END,
		KEY::CAPSLOCK,
		KEY::SCROLL_LOCK,
		KEY::NUM_LOCK,
		KEY::PRINT_SCREEN,
		KEY::PAUSE,
		KEY::F1,
		KEY::F2,
		KEY::F3,
		KEY::F4,
		KEY::F5,
		KEY::F6,
		KEY::F7,
		KEY::F8,
		KEY::F9,
		KEY::F10,
		KEY::F11,
		KEY::F12,
		KEY::F13,
		KEY::F14,
		KEY::F15,
		KEY::F16,
		KEY::F17,
		KEY::F18,
		KEY::F19,
		KEY::F20,
		KEY::F21,
		KEY::F22,
		KEY::F23,
		KEY::F24,
		KEY::F25,
		KEY::KP0,
		KEY::KP1,
		KEY::KP2,
		KEY::KP3,
		KEY::KP4,
		KEY::KP5,
		KEY::KP6,
		KEY::KP7,
		KEY::KP8,
		KEY::KP9,
		KEY::KP_DECIMAL,
		KEY::KP_DIVIDE,
		KEY::KP_MULTIPLY,
		KEY::KP_SUBTRACT,
		KEY::KP_ADD,
		KEY::KP_ENTER,
		KEY::KP_EQUAL,
		KEY::LEFT_SHIFT,
		KEY::LEFT_CONTROL,
		KEY::LEFT_ALT,
		KEY::LEFT_SUPER,
		KEY::RIGHT_SHIFT,
		KEY::RIGHT_CONTROL,
		KEY::RIGHT_ALT,
		KEY::RIGHT_SUPER,
		KEY::MENU
	};

	// --- Mouse ---
	static std::vector<MOUSE_CODE> s_MouseButtonsVec =
	{
		MOUSE::BUTTON0,
		MOUSE::BUTTON1,
		MOUSE::BUTTON2,
		MOUSE::BUTTON3,
		MOUSE::BUTTON4,
		MOUSE::BUTTON5,
		MOUSE::BUTTON6,
		MOUSE::BUTTON7
	};

	// --- Joystick ---
	static std::vector<JOYSTICK_CODE> s_JoystickButtonsVec =
	{
		JOYSTICK::JOYSTICK0,
		JOYSTICK::JOYSTICK1,
		JOYSTICK::JOYSTICK2,
		JOYSTICK::JOYSTICK3,
		JOYSTICK::JOYSTICK4,
		JOYSTICK::JOYSTICK5,
		JOYSTICK::JOYSTICK6,
		JOYSTICK::JOYSTICK7,
		JOYSTICK::JOYSTICK8,
		JOYSTICK::JOYSTICK9,
		JOYSTICK::JOYSTICK10,
		JOYSTICK::JOYSTICK11,
		JOYSTICK::JOYSTICK12,
		JOYSTICK::JOYSTICK13,
		JOYSTICK::JOYSTICK14,
		JOYSTICK::JOYSTICK15
	};

	// --- Gamepad ---
	static std::vector<GAMEPAD_CODE> s_GamepadButtonsVec =
	{
		GAMEPAD::GAMEPAD_BTN_A,
		GAMEPAD::GAMEPAD_BTN_B,
		GAMEPAD::GAMEPAD_BTN_X,
		GAMEPAD::GAMEPAD_BTN_Y,
		GAMEPAD::GAMEPAD_BTN_LBUMP,
		GAMEPAD::GAMEPAD_BTN_RBUMP,
		GAMEPAD::GAMEPAD_BTN_BACK,
		GAMEPAD::GAMEPAD_BTN_START,
		GAMEPAD::GAMEPAD_BTN_GUIDE,
		GAMEPAD::GAMEPAD_BTN_LTHUMB,
		GAMEPAD::GAMEPAD_BTN_RTHUMB,
		GAMEPAD::GAMEPAD_BTNDP_UP,
		GAMEPAD::GAMEPAD_BTNDP_RIGHT,
		GAMEPAD::GAMEPAD_BTNDP_DOWN,
		GAMEPAD::GAMEPAD_BTNDP_LEFT
	};

	// --- Gamepad Axis ---
	static std::vector<GAMEPADAXIS_CODE> s_GamepadAxisVec =
	{
		GAMEPADAXIS::GAMEPAD_AXIS_LX,
		GAMEPADAXIS::GAMEPAD_AXIS_LY,
		GAMEPADAXIS::GAMEPAD_AXIS_RX,
		GAMEPADAXIS::GAMEPAD_AXIS_RY,
		GAMEPADAXIS::GAMEPAD_AXIS_LTRIGGER,
		GAMEPADAXIS::GAMEPAD_AXIS_RTRIGGER
	};
}


// ---------------------------------------
// ---------------------------------------
#endif //_KAIMOSINPUTCODES_H_
