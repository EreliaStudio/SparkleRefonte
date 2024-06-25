#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>
#include "structure/spk_safe_pointer.hpp"

#include "structure/system/device/spk_mouse.hpp"
#include "structure/system/device/spk_keyboard.hpp"
#include "structure/system/device/spk_controller.hpp"

namespace spk
{
	class Window;

	struct Modifiers
	{
		bool control;
		bool alt;
		bool shift;
	};

	struct Event
	{
		bool consumed;
		Modifiers modifiers;

		union Carriage
		{
			struct PaintCarriage
			{
				enum class Type
				{
					OnFocus,
					OnResize,
					Requested
				};
				Type type;

				spk::SafePointer<Window> window;
			} paintCarriage;

			struct UpdateCarriage
			{
				enum class Type
				{
					Requested
				};
				Type type;

				long long deltaTime;
				long long time;
			} updateCarriage;
			struct MouseCarriage
			{
				enum class Type
				{
					Press,
					Release,
					Motion,
					Wheel
				};

				Type type;
				union
				{
					spk::Mouse::Button button;
					spk::Vector2Int position;
					float scrollValue;
				};
			} mouseCarriage;

			struct KeyboardCarriage
			{
				enum class Type
				{
					Press,
					Release
				};
				Type type;

				union
				{
					spk::Keyboard::Key key;
					wchar_t glyph;
				};
			} keyboardCarriage;

			struct ControllerCarriage
			{
				enum class Type
				{
					Press,
					Release,
					Motion
				};
				Type type;

				union
				{
					spk::Controller::Button button;
					struct 
					{
						spk::Controller::Joystick::ID id;
						spk::Vector2 values;
					} joystick;
				};
			} controllerCarriage;

			struct SystemCarriage
			{
				enum class Type
				{
					Focus,
					Quit,
					Move
				};
				Type type;
			} systemCarriage;

			Carriage()
			{

			}
		} carriage;

		Event()
		{

		}

		Event(spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			construct(p_window, uMsg, wParam, lParam);
		}

		bool construct(spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			consumed = false;

			switch (uMsg)
			{
			case WM_PAINT:
			{
				carriage.paintCarriage.type = Carriage::PaintCarriage::Type::Requested;
				carriage.paintCarriage.window = p_window;
				break;
			}
			case WM_SIZE:
			{
				carriage.paintCarriage.type = Carriage::PaintCarriage::Type::OnResize;
				carriage.paintCarriage.window = p_window;
				break;
			}
			case WM_LBUTTONDOWN:
			{
				carriage.mouseCarriage.type = Carriage::MouseCarriage::Type::Press;
				carriage.mouseCarriage.button = spk::Mouse::Button::Left;
				break;
			}
			case WM_RBUTTONDOWN:
			{
				carriage.mouseCarriage.type = Carriage::MouseCarriage::Type::Press;
				carriage.mouseCarriage.button = spk::Mouse::Button::Right;
				break;
			}
			case WM_MBUTTONDOWN:
			{
				carriage.mouseCarriage.type = Carriage::MouseCarriage::Type::Press;
				carriage.mouseCarriage.button = spk::Mouse::Button::Middle;
				break;
			}
			case WM_LBUTTONUP:
			{
				carriage.mouseCarriage.type = Carriage::MouseCarriage::Type::Release;
				carriage.mouseCarriage.button = spk::Mouse::Button::Left;
				break;
			}
			case WM_RBUTTONUP:
			{
				carriage.mouseCarriage.type = Carriage::MouseCarriage::Type::Release;
				carriage.mouseCarriage.button = spk::Mouse::Button::Right;
				break;
			}
			case WM_MBUTTONUP:
			{
				carriage.mouseCarriage.type = Carriage::MouseCarriage::Type::Release;
				carriage.mouseCarriage.button = spk::Mouse::Button::Middle;
				break;
			}
			case WM_MOUSEMOVE:
			{
				carriage.mouseCarriage.type = Carriage::MouseCarriage::Type::Motion;
				carriage.mouseCarriage.position = spk::Vector2Int{ LOWORD(wParam), HIWORD(wParam) };
				break;
			}
			case WM_MOUSEWHEEL:
			{
				carriage.mouseCarriage.type = Carriage::MouseCarriage::Type::Wheel;
				carriage.mouseCarriage.scrollValue = GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
				break;
			}
			case WM_KEYDOWN:
			{
				carriage.keyboardCarriage.type = Carriage::KeyboardCarriage::Type::Press;
				carriage.keyboardCarriage.key = static_cast<spk::Keyboard::Key>(wParam);
				break;
			}
			case WM_KEYUP:
			{
				carriage.keyboardCarriage.type = Carriage::KeyboardCarriage::Type::Release;
				carriage.keyboardCarriage.key = static_cast<spk::Keyboard::Key>(wParam);
				break;
			}
			case WM_CHAR:
			{
				carriage.keyboardCarriage.type = Carriage::KeyboardCarriage::Type::Press;
				carriage.keyboardCarriage.glyph = static_cast<wchar_t>(wParam);
				break;
			}
			case WM_SETFOCUS:
			{
				carriage.systemCarriage.type = Carriage::SystemCarriage::Type::Focus;
				break;
			}
			case WM_KILLFOCUS:
			{
				carriage.systemCarriage.type = Carriage::SystemCarriage::Type::Focus;
				break;
			}
			case WM_CLOSE:
			case WM_QUIT:
			{
				carriage.systemCarriage.type = Carriage::SystemCarriage::Type::Quit;
				break;
			}
			case WM_MOVE:
			{
				carriage.systemCarriage.type = Carriage::SystemCarriage::Type::Move;
				break;
			}
			default:
				return false;
			}

			// Set modifiers
			modifiers.control = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
			modifiers.alt = (GetKeyState(VK_MENU) & 0x8000) != 0;
			modifiers.shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;

			return true;
		}

	};


}