#ifndef _KAIMOSINPUTCODES_H_
#define _KAIMOSINPUTCODES_H_

// ------------ Kaimos Engine redefinition for Input Codes, from glfw3.h ------------



/* The unknown key */
#define KS_KEY_UNKNOWN            -1



// ------------ KEYBOARD CODES ------------
/* -- Printable keys -- */
#define KS_KEY_SPACE              32
#define KS_KEY_APOSTROPHE         39  /* ' */
#define KS_KEY_COMMA              44  /* , */
#define KS_KEY_MINUS              45  /* - */
#define KS_KEY_PERIOD             46  /* . */
#define KS_KEY_SLASH              47  /* / */
#define KS_KEY_0                  48
#define KS_KEY_1                  49
#define KS_KEY_2                  50
#define KS_KEY_3                  51
#define KS_KEY_4                  52
#define KS_KEY_5                  53
#define KS_KEY_6                  54
#define KS_KEY_7                  55
#define KS_KEY_8                  56
#define KS_KEY_9                  57
#define KS_KEY_SEMICOLON          59  /* ; */
#define KS_KEY_EQUAL              61  /* = */
#define KS_KEY_A                  65
#define KS_KEY_B                  66
#define KS_KEY_C                  67
#define KS_KEY_D                  68
#define KS_KEY_E                  69
#define KS_KEY_F                  70
#define KS_KEY_G                  71
#define KS_KEY_H                  72
#define KS_KEY_I                  73
#define KS_KEY_J                  74
#define KS_KEY_K                  75
#define KS_KEY_L                  76
#define KS_KEY_M                  77
#define KS_KEY_N                  78
#define KS_KEY_O                  79
#define KS_KEY_P                  80
#define KS_KEY_Q                  81
#define KS_KEY_R                  82
#define KS_KEY_S                  83
#define KS_KEY_T                  84
#define KS_KEY_U                  85
#define KS_KEY_V                  86
#define KS_KEY_W                  87
#define KS_KEY_X                  88
#define KS_KEY_Y                  89
#define KS_KEY_Z                  90
#define KS_KEY_LEFT_BRACKET       91  /* [ */
#define KS_KEY_BACKSLASH          92  /* \ */
#define KS_KEY_RIGHT_BRACKET      93  /* ] */
#define KS_KEY_GRAVE_ACCENT       96  /* ` */
#define KS_KEY_WORLD_1            161 /* non-US #1 */
#define KS_KEY_WORLD_2            162 /* non-US #2 */

/* -- Function keys -- */
#define KS_KEY_ESCAPE             256
#define KS_KEY_ENTER              257
#define KS_KEY_TAB                258
#define KS_KEY_BACKSPACE          259
#define KS_KEY_INSERT             260
#define KS_KEY_DELETE             261
#define KS_KEY_RIGHT              262
#define KS_KEY_LEFT               263
#define KS_KEY_DOWN               264
#define KS_KEY_UP                 265
#define KS_KEY_PAGE_UP            266
#define KS_KEY_PAGE_DOWN          267
#define KS_KEY_HOME               268
#define KS_KEY_END                269
#define KS_KEY_CAPS_LOCK          280
#define KS_KEY_SCROLL_LOCK        281
#define KS_KEY_NUM_LOCK           282
#define KS_KEY_PRINT_SCREEN       283
#define KS_KEY_PAUSE              284
#define KS_KEY_F1                 290
#define KS_KEY_F2                 291
#define KS_KEY_F3                 292
#define KS_KEY_F4                 293
#define KS_KEY_F5                 294
#define KS_KEY_F6                 295
#define KS_KEY_F7                 296
#define KS_KEY_F8                 297
#define KS_KEY_F9                 298
#define KS_KEY_F10                299
#define KS_KEY_F11                300
#define KS_KEY_F12                301
#define KS_KEY_F13                302
#define KS_KEY_F14                303
#define KS_KEY_F15                304
#define KS_KEY_F16                305
#define KS_KEY_F17                306
#define KS_KEY_F18                307
#define KS_KEY_F19                308
#define KS_KEY_F20                309
#define KS_KEY_F21                310
#define KS_KEY_F22                311
#define KS_KEY_F23                312
#define KS_KEY_F24                313
#define KS_KEY_F25                314
#define KS_KEY_KP_0               320
#define KS_KEY_KP_1               321
#define KS_KEY_KP_2               322
#define KS_KEY_KP_3               323
#define KS_KEY_KP_4               324
#define KS_KEY_KP_5               325
#define KS_KEY_KP_6               326
#define KS_KEY_KP_7               327
#define KS_KEY_KP_8               328
#define KS_KEY_KP_9               329
#define KS_KEY_KP_DECIMAL         330
#define KS_KEY_KP_DIVIDE          331
#define KS_KEY_KP_MULTIPLY        332
#define KS_KEY_KP_SUBTRACT        333
#define KS_KEY_KP_ADD             334
#define KS_KEY_KP_ENTER           335
#define KS_KEY_KP_EQUAL           336
#define KS_KEY_LEFT_SHIFT         340
#define KS_KEY_LEFT_CONTROL       341
#define KS_KEY_LEFT_ALT           342
#define KS_KEY_LEFT_SUPER         343
#define KS_KEY_RIGHT_SHIFT        344
#define KS_KEY_RIGHT_CONTROL      345
#define KS_KEY_RIGHT_ALT          346
#define KS_KEY_RIGHT_SUPER        347
#define KS_KEY_MENU               348

#define KS_KEY_LAST               KS_KEY_MENU

/* -- Modifier keys -- */
// MOD = Modifier key flags
// @brief If this bit is set one or more Shift keys were held down.
#define KS_MOD_SHIFT           0x0001
// @brief If this bit is set one or more Control keys were held down.
#define KS_MOD_CONTROL         0x0002
// @brief If this bit is set one or more Alt keys were held down.
#define KS_MOD_ALT             0x0004
//@brief If this bit is set one or more Super keys were held down.
#define KS_MOD_SUPER           0x0008
//@brief If this bit is set the Caps Lock key is enabled.
#define KS_MOD_CAPS_LOCK       0x0010
//@brief If this bit is set the Num Lock key is enabled.
#define KS_MOD_NUM_LOCK        0x0020



// ------------ MOUSE CODES ------------
#define KS_MOUSE_BUTTON_1         0
#define KS_MOUSE_BUTTON_2         1
#define KS_MOUSE_BUTTON_3         2
#define KS_MOUSE_BUTTON_4         3
#define KS_MOUSE_BUTTON_5         4
#define KS_MOUSE_BUTTON_6         5
#define KS_MOUSE_BUTTON_7         6
#define KS_MOUSE_BUTTON_8         7
#define KS_MOUSE_BUTTON_LAST      KS_MOUSE_BUTTON_8
#define KS_MOUSE_BUTTON_LEFT      KS_MOUSE_BUTTON_1
#define KS_MOUSE_BUTTON_RIGHT     KS_MOUSE_BUTTON_2
#define KS_MOUSE_BUTTON_MIDDLE    KS_MOUSE_BUTTON_3



// ------------ JOYSTICK CODES ------------
#define KS_JOYSTICK_1             0
#define KS_JOYSTICK_2             1
#define KS_JOYSTICK_3             2
#define KS_JOYSTICK_4             3
#define KS_JOYSTICK_5             4
#define KS_JOYSTICK_6             5
#define KS_JOYSTICK_7             6
#define KS_JOYSTICK_8             7
#define KS_JOYSTICK_9             8
#define KS_JOYSTICK_10            9
#define KS_JOYSTICK_11            10
#define KS_JOYSTICK_12            11
#define KS_JOYSTICK_13            12
#define KS_JOYSTICK_14            13
#define KS_JOYSTICK_15            14
#define KS_JOYSTICK_16            15
#define KS_JOYSTICK_LAST          KS_JOYSTICK_16



// ------------ GAMEPAD CODES ------------
#define KS_GAMEPAD_BUTTON_A               0
#define KS_GAMEPAD_BUTTON_B               1
#define KS_GAMEPAD_BUTTON_X               2
#define KS_GAMEPAD_BUTTON_Y               3
#define KS_GAMEPAD_BUTTON_LEFT_BUMPER     4
#define KS_GAMEPAD_BUTTON_RIGHT_BUMPER    5
#define KS_GAMEPAD_BUTTON_BACK            6
#define KS_GAMEPAD_BUTTON_START           7
#define KS_GAMEPAD_BUTTON_GUIDE           8
#define KS_GAMEPAD_BUTTON_LEFT_THUMB      9
#define KS_GAMEPAD_BUTTON_RIGHT_THUMB     10
#define KS_GAMEPAD_BUTTON_DPAD_UP         11
#define KS_GAMEPAD_BUTTON_DPAD_RIGHT      12
#define KS_GAMEPAD_BUTTON_DPAD_DOWN       13
#define KS_GAMEPAD_BUTTON_DPAD_LEFT       14
#define KS_GAMEPAD_BUTTON_LAST            KS_GAMEPAD_BUTTON_DPAD_LEFT

#define KS_GAMEPAD_BUTTON_CROSS       KS_GAMEPAD_BUTTON_A
#define KS_GAMEPAD_BUTTON_CIRCLE      KS_GAMEPAD_BUTTON_B
#define KS_GAMEPAD_BUTTON_SQUARE      KS_GAMEPAD_BUTTON_X
#define KS_GAMEPAD_BUTTON_TRIANGLE    KS_GAMEPAD_BUTTON_Y

/* -- Gamepad Axis -- */
#define KS_GAMEPAD_AXIS_LEFT_X        0
#define KS_GAMEPAD_AXIS_LEFT_Y        1
#define KS_GAMEPAD_AXIS_RIGHT_X       2
#define KS_GAMEPAD_AXIS_RIGHT_Y       3
#define KS_GAMEPAD_AXIS_LEFT_TRIGGER  4
#define KS_GAMEPAD_AXIS_RIGHT_TRIGGER 5
#define KS_GAMEPAD_AXIS_LAST          KS_GAMEPAD_AXIS_RIGHT_TRIGGER



// ---------------------------------------
// ---------------------------------------
#endif // _KAIMOSINPUTCODES_H_