#ifndef _KAIMOSINPUTCODES_H_
#define _KAIMOSINPUTCODES_H_

// ------------ Kaimos Engine redefinition for Input Codes, from glfw3.h ------------
namespace Kaimos
{
	using KEYCODE = uint16_t;
	namespace Key
	{
		enum KEYCODE
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
	using MOUSECODE = uint16_t;
	namespace Mouse
	{
		enum KEYCODE
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
		};
	}
}



// ------------ JOYSTICK CODES ------------
namespace Kaimos
{
	using JOYSTICKCODE = uint16_t;
	namespace Joystick
	{
		enum JOYSTICKCODE
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
		};
	}
}



// ------------ GAMEPAD CODES ------------
namespace Kaimos
{
	using GAMEPADCODE = uint16_t;
	namespace Gamepad
	{
		enum GAMEPADCODE
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
			GamepadBtnDP_Left = 14,

			GamepadBtnLast = GamepadBtnDP_Left,
			GamepadBtnCross = GamepadBtnA,
			GamepadBtnCircle = GamepadBtnB,
			GamepadBtnSquare = GamepadBtnX,
			GamepadBtnTraingle = GamepadBtnY
		};
	}
}



/* -- Gamepad Axis -- */
namespace Kaimos
{
	using GAMEPADAXISCODE = uint16_t;
	namespace GamepadAxis
	{
		enum GAMEPADAXISCODE
		{
			GamepadAxis_LX = 0,
			GamepadAxis_LY = 1,
			GamepadAxis_RX = 2,
			GamepadAxis_RY = 3,
			GamepadAxis_LTrigger = 4,
			GamepadAxis_RTrigger = 5,
			GamepadAxis_Last = GamepadAxis_RTrigger
		};
	}
}



// ---------------------------------------
// ---------------------------------------
#endif // _KAIMOSINPUTCODES_H_
