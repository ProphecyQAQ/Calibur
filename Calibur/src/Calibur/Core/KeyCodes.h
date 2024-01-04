#pragma once

namespace Calibur
{
	typedef enum class KeyCode : uint16_t
	{
		// From glfw3.h
		Space = 32,
		Apostrophe = 39, /* ' */
		Comma = 44, /* , */
		Minus = 45, /* - */
		Period = 46, /* . */
		Slash = 47, /* / */

		D0 = 48, /* 0 */
		D1 = 49, /* 1 */
		D2 = 50, /* 2 */
		D3 = 51, /* 3 */
		D4 = 52, /* 4 */
		D5 = 53, /* 5 */
		D6 = 54, /* 6 */
		D7 = 55, /* 7 */
		D8 = 56, /* 8 */
		D9 = 57, /* 9 */

		Semicolon = 59, /* ; */
		Equal = 61, /* = */

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

		LeftBracket = 91,  /* [ */
		Backslash = 92,  /* \ */
		RightBracket = 93,  /* ] */
		GraveAccent = 96,  /* ` */

		World1 = 161, /* non-US #1 */
		World2 = 162, /* non-US #2 */

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

	inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
	{
		os << static_cast<int32_t>(keyCode);
		return os;
	}
}

#define HZ_KEY_SPACE           ::Calibur::Key::Space
#define HZ_KEY_APOSTROPHE      ::Calibur::Key::Apostrophe    /* ' */
#define HZ_KEY_COMMA           ::Calibur::Key::Comma         /* , */
#define HZ_KEY_MINUS           ::Calibur::Key::Minus         /* - */
#define HZ_KEY_PERIOD          ::Calibur::Key::Period        /* . */
#define HZ_KEY_SLASH           ::Calibur::Key::Slash         /* / */
#define HZ_KEY_0               ::Calibur::Key::D0
#define HZ_KEY_1               ::Calibur::Key::D1
#define HZ_KEY_2               ::Calibur::Key::D2
#define HZ_KEY_3               ::Calibur::Key::D3
#define HZ_KEY_4               ::Calibur::Key::D4
#define HZ_KEY_5               ::Calibur::Key::D5
#define HZ_KEY_6               ::Calibur::Key::D6
#define HZ_KEY_7               ::Calibur::Key::D7
#define HZ_KEY_8               ::Calibur::Key::D8
#define HZ_KEY_9               ::Calibur::Key::D9
#define HZ_KEY_SEMICOLON       ::Calibur::Key::Semicolon     /* ; */
#define HZ_KEY_EQUAL           ::Calibur::Key::Equal         /* = */
#define HZ_KEY_A               ::Calibur::Key::A
#define HZ_KEY_B               ::Calibur::Key::B
#define HZ_KEY_C               ::Calibur::Key::C
#define HZ_KEY_D               ::Calibur::Key::D
#define HZ_KEY_E               ::Calibur::Key::E
#define HZ_KEY_F               ::Calibur::Key::F
#define HZ_KEY_G               ::Calibur::Key::G
#define HZ_KEY_H               ::Calibur::Key::H
#define HZ_KEY_I               ::Calibur::Key::I
#define HZ_KEY_J               ::Calibur::Key::J
#define HZ_KEY_K               ::Calibur::Key::K
#define HZ_KEY_L               ::Calibur::Key::L
#define HZ_KEY_M               ::Calibur::Key::M
#define HZ_KEY_N               ::Calibur::Key::N
#define HZ_KEY_O               ::Calibur::Key::O
#define HZ_KEY_P               ::Calibur::Key::P
#define HZ_KEY_Q               ::Calibur::Key::Q
#define HZ_KEY_R               ::Calibur::Key::R
#define HZ_KEY_S               ::Calibur::Key::S
#define HZ_KEY_T               ::Calibur::Key::T
#define HZ_KEY_U               ::Calibur::Key::U
#define HZ_KEY_V               ::Calibur::Key::V
#define HZ_KEY_W               ::Calibur::Key::W
#define HZ_KEY_X               ::Calibur::Key::X
#define HZ_KEY_Y               ::Calibur::Key::Y
#define HZ_KEY_Z               ::Calibur::Key::Z
#define HZ_KEY_LEFT_BRACKET    ::Calibur::Key::LeftBracket   /* [ */
#define HZ_KEY_BACKSLASH       ::Calibur::Key::Backslash     /* \ */
#define HZ_KEY_RIGHT_BRACKET   ::Calibur::Key::RightBracket  /* ] */
#define HZ_KEY_GRAVE_ACCENT    ::Calibur::Key::GraveAccent   /* ` */
#define HZ_KEY_WORLD_1         ::Calibur::Key::World1        /* non-US #1 */
#define HZ_KEY_WORLD_2         ::Calibur::Key::World2        /* non-US #2 */

#define HZ_KEY_ESCAPE          ::Calibur::Key::Escape
#define HZ_KEY_ENTER           ::Calibur::Key::Enter
#define HZ_KEY_TAB             ::Calibur::Key::Tab
#define HZ_KEY_BACKSPACE       ::Calibur::Key::Backspace
#define HZ_KEY_INSERT          ::Calibur::Key::Insert
#define HZ_KEY_DELETE          ::Calibur::Key::Delete
#define HZ_KEY_RIGHT           ::Calibur::Key::Right
#define HZ_KEY_LEFT            ::Calibur::Key::Left
#define HZ_KEY_DOWN            ::Calibur::Key::Down
#define HZ_KEY_UP              ::Calibur::Key::Up
#define HZ_KEY_PAGE_UP         ::Calibur::Key::PageUp
#define HZ_KEY_PAGE_DOWN       ::Calibur::Key::PageDown
#define HZ_KEY_HOME            ::Calibur::Key::Home
#define HZ_KEY_END             ::Calibur::Key::End
#define HZ_KEY_CAPS_LOCK       ::Calibur::Key::CapsLock
#define HZ_KEY_SCROLL_LOCK     ::Calibur::Key::ScrollLock
#define HZ_KEY_NUM_LOCK        ::Calibur::Key::NumLock
#define HZ_KEY_PRINT_SCREEN    ::Calibur::Key::PrintScreen
#define HZ_KEY_PAUSE           ::Calibur::Key::Pause
#define HZ_KEY_F1              ::Calibur::Key::F1
#define HZ_KEY_F2              ::Calibur::Key::F2
#define HZ_KEY_F3              ::Calibur::Key::F3
#define HZ_KEY_F4              ::Calibur::Key::F4
#define HZ_KEY_F5              ::Calibur::Key::F5
#define HZ_KEY_F6              ::Calibur::Key::F6
#define HZ_KEY_F7              ::Calibur::Key::F7
#define HZ_KEY_F8              ::Calibur::Key::F8
#define HZ_KEY_F9              ::Calibur::Key::F9
#define HZ_KEY_F10             ::Calibur::Key::F10
#define HZ_KEY_F11             ::Calibur::Key::F11
#define HZ_KEY_F12             ::Calibur::Key::F12
#define HZ_KEY_F13             ::Calibur::Key::F13
#define HZ_KEY_F14             ::Calibur::Key::F14
#define HZ_KEY_F15             ::Calibur::Key::F15
#define HZ_KEY_F16             ::Calibur::Key::F16
#define HZ_KEY_F17             ::Calibur::Key::F17
#define HZ_KEY_F18             ::Calibur::Key::F18
#define HZ_KEY_F19             ::Calibur::Key::F19
#define HZ_KEY_F20             ::Calibur::Key::F20
#define HZ_KEY_F21             ::Calibur::Key::F21
#define HZ_KEY_F22             ::Calibur::Key::F22
#define HZ_KEY_F23             ::Calibur::Key::F23
#define HZ_KEY_F24             ::Calibur::Key::F24
#define HZ_KEY_F25             ::Calibur::Key::F25

/* Keypad */
#define HZ_KEY_KP_0            ::Calibur::Key::KP0
#define HZ_KEY_KP_1            ::Calibur::Key::KP1
#define HZ_KEY_KP_2            ::Calibur::Key::KP2
#define HZ_KEY_KP_3            ::Calibur::Key::KP3
#define HZ_KEY_KP_4            ::Calibur::Key::KP4
#define HZ_KEY_KP_5            ::Calibur::Key::KP5
#define HZ_KEY_KP_6            ::Calibur::Key::KP6
#define HZ_KEY_KP_7            ::Calibur::Key::KP7
#define HZ_KEY_KP_8            ::Calibur::Key::KP8
#define HZ_KEY_KP_9            ::Calibur::Key::KP9
#define HZ_KEY_KP_DECIMAL      ::Calibur::Key::KPDecimal
#define HZ_KEY_KP_DIVIDE       ::Calibur::Key::KPDivide
#define HZ_KEY_KP_MULTIPLY     ::Calibur::Key::KPMultiply
#define HZ_KEY_KP_SUBTRACT     ::Calibur::Key::KPSubtract
#define HZ_KEY_KP_ADD          ::Calibur::Key::KPAdd
#define HZ_KEY_KP_ENTER        ::Calibur::Key::KPEnter
#define HZ_KEY_KP_EQUAL        ::Calibur::Key::KPEqual

#define HZ_KEY_LEFT_SHIFT      ::Calibur::Key::LeftShift
#define HZ_KEY_LEFT_CONTROL    ::Calibur::Key::LeftControl
#define HZ_KEY_LEFT_ALT        ::Calibur::Key::LeftAlt
#define HZ_KEY_LEFT_SUPER      ::Calibur::Key::LeftSuper
#define HZ_KEY_RIGHT_SHIFT     ::Calibur::Key::RightShift
#define HZ_KEY_RIGHT_CONTROL   ::Calibur::Key::RightControl
#define HZ_KEY_RIGHT_ALT       ::Calibur::Key::RightAlt
#define HZ_KEY_RIGHT_SUPER     ::Calibur::Key::RightSuper
#define HZ_KEY_MENU            ::Calibur::Key::Menu
