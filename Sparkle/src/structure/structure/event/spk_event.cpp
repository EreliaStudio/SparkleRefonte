#include "structure/system/event/spk_event.hpp"

#include "structure/graphical/spk_window.hpp"

#include "spk_debug_macro.hpp"

namespace spk
{
	Event::Event() {}

	Event::Event(spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		construct(p_window, uMsg, wParam, lParam);
	}
	
	const std::unordered_map<UINT, Event::ConstructorLambda> Event::_constructionMap =
	{
		{
			WM_PAINT,
			[&](Event* p_event, spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
				p_event->paintEvent.type = PaintEvent::Type::Requested;
				p_event->paintEvent.window = p_window;
			}
		},
		{
			WM_LBUTTONDOWN,
			[&](Event* p_event, spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
				p_event->mouseEvent.type = MouseEvent::Type::Press;
				p_event->mouseEvent.button = spk::Mouse::Button::Left;
			}
		},
		{
			WM_RBUTTONDOWN,
			[&](Event* p_event, spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
				p_event->mouseEvent.type = MouseEvent::Type::Press;
				p_event->mouseEvent.button = spk::Mouse::Button::Right;
			}
		},
		{
			WM_MBUTTONDOWN,
			[&](Event* p_event, spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
				p_event->mouseEvent.type = MouseEvent::Type::Press;
				p_event->mouseEvent.button = spk::Mouse::Button::Middle;
			}
		},
		{
			WM_LBUTTONUP,
			[&](Event* p_event, spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
				p_event->mouseEvent.type = MouseEvent::Type::Release;
				p_event->mouseEvent.button = spk::Mouse::Button::Left;
			}
		},
		{
			WM_RBUTTONUP,
			[&](Event* p_event, spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
				p_event->mouseEvent.type = MouseEvent::Type::Release;
				p_event->mouseEvent.button = spk::Mouse::Button::Right;
			}
		},
		{
			WM_MBUTTONUP,
			[&](Event* p_event, spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
				p_event->mouseEvent.type = MouseEvent::Type::Release;
				p_event->mouseEvent.button = spk::Mouse::Button::Middle;
			}
		},
		{
			WM_LBUTTONDBLCLK,
			[&](Event* p_event, spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
				p_event->mouseEvent.type = MouseEvent::Type::DoubleClick;
				p_event->mouseEvent.button = spk::Mouse::Button::Left;
			}
		},
		{
			WM_RBUTTONDBLCLK,
			[&](Event* p_event, spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
				p_event->mouseEvent.type = MouseEvent::Type::DoubleClick;
				p_event->mouseEvent.button = spk::Mouse::Button::Right;
			}
		},
		{
			WM_MBUTTONDBLCLK,
			[&](Event* p_event, spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
				p_event->mouseEvent.type = MouseEvent::Type::DoubleClick;
				p_event->mouseEvent.button = spk::Mouse::Button::Middle;
			}
		},
		{
			WM_MOUSEMOVE,
			[&](Event* p_event, spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
				p_event->mouseEvent.type = MouseEvent::Type::Motion;
				p_event->mouseEvent.position = spk::Vector2Int{ LOWORD(wParam), HIWORD(lParam) };
			}
		},
		{
			WM_MOUSEWHEEL,
			[&](Event* p_event, spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
				p_event->mouseEvent.type = MouseEvent::Type::Wheel;
				p_event->mouseEvent.scrollValue = GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
			}
		},
		{
			WM_KEYDOWN,
			[&](Event* p_event, spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
				p_event->keyboardEvent.type = KeyboardEvent::Type::Press;
				p_event->keyboardEvent.key = static_cast<spk::Keyboard::Key>(wParam);
			}
		},
		{
			WM_KEYUP,
			[&](Event* p_event, spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
				p_event->keyboardEvent.type = KeyboardEvent::Type::Release;
				p_event->keyboardEvent.key = static_cast<spk::Keyboard::Key>(wParam);
			}
		},
		{
			WM_CHAR,
			[&](Event* p_event, spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
				p_event->keyboardEvent.type = KeyboardEvent::Type::Press;
				p_event->keyboardEvent.glyph = static_cast<wchar_t>(wParam);
			}
		},
		{
			WM_SIZE,
			[&](Event* p_event, spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
				p_event->systemEvent.type = SystemEvent::Type::Resize;
				p_event->systemEvent.window = p_window;
				p_event->systemEvent.newSize = spk::Geometry2DInt::Size(LOWORD(lParam), HIWORD(lParam));
			}
		},
		{
			WM_SETFOCUS,
			[&](Event* p_event, spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
				p_event->systemEvent.type = SystemEvent::Type::TakeFocus;
			}
		},
		{
			WM_KILLFOCUS,
			[&](Event* p_event, spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
				p_event->systemEvent.type = SystemEvent::Type::LoseFocus;
			}
		},
		{
			WM_CLOSE,
			[&](Event* p_event, spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
				p_event->systemEvent.type = SystemEvent::Type::Quit;
				p_event->systemEvent.window = p_window;
			}
		},
		{
			WM_QUIT,
			[&](Event* p_event, spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
				p_event->systemEvent.type = SystemEvent::Type::Quit;
				p_event->systemEvent.window = p_window;
			}
		},
		{
			WM_MOVE,
			[&](Event* p_event, spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
				p_event->systemEvent.type = SystemEvent::Type::Move;
				p_event->systemEvent.window = p_window;
				p_event->systemEvent.newPosition = spk::Geometry2DInt::Position(LOWORD(lParam), HIWORD(lParam));
			}
		},
		{
			WM_LEFT_JOYSTICK_MOVE,
			[&](Event* p_event, spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
				p_event->controllerEvent.type = ControllerEvent::Type::Motion;
				p_event->controllerEvent.joystick.id = Controller::Joystick::ID::Left;
				p_event->controllerEvent.joystick.values = spk::Vector2Int(LOWORD(lParam), HIWORD(lParam));
			}
		}
	};

	bool Event::construct(spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (_constructionMap.contains(uMsg) == false)
			return (false);
		if (uMsg == WM_PAINT)
			ValidateRect(p_window->_hwnd, NULL);

		spk::cout << "Event id : " << WM_CONTROLLER_BUTTON_PRESS << " / " << WM_CONTROLLER_BUTTON_RELEASE << " / " << WM_LEFT_JOYSTICK_MOVE << " / " << WM_RIGHT_JOYSTICK_MOVE << std::endl;
		setModifiers(uMsg);
		_constructionMap.at(uMsg)(this, p_window, uMsg, wParam, lParam);
		return (true);
	}

	void Event::setModifiers(UINT uMsg)
	{
		IEvent* currentEvent = nullptr;

		switch (uMsg)
		{
		case WM_PAINT:
			currentEvent = &paintEvent;
			break;

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
		case WM_MOUSEMOVE:
		case WM_MOUSEWHEEL:
			currentEvent = &mouseEvent;
			break;

		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
			currentEvent = &keyboardEvent;
			break;

		case WM_SIZE:
		case WM_SETFOCUS:
		case WM_KILLFOCUS:
		case WM_CLOSE:
		case WM_QUIT:
		case WM_MOVE:
			currentEvent = &systemEvent;
			break;

		default:
			return;
		}

		if (currentEvent)
		{
			currentEvent->modifiers.control = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
			currentEvent->modifiers.alt = (GetKeyState(VK_MENU) & 0x8000) != 0;
			currentEvent->modifiers.shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
		}
	}
}