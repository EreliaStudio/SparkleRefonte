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

	bool Event::construct(spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_PAINT:
			paintEvent.type = PaintEvent::Type::Requested;
			paintEvent.window = p_window;
			ValidateRect(p_window->_hwnd, NULL);
			break;

		case WM_LBUTTONDOWN:
			mouseEvent.type = MouseEvent::Type::Press;
			mouseEvent.button = spk::Mouse::Button::Left;
			break;

		case WM_RBUTTONDOWN:
			mouseEvent.type = MouseEvent::Type::Press;
			mouseEvent.button = spk::Mouse::Button::Right;
			break;

		case WM_MBUTTONDOWN:
			mouseEvent.type = MouseEvent::Type::Press;
			mouseEvent.button = spk::Mouse::Button::Middle;
			break;

		case WM_LBUTTONUP:
			mouseEvent.type = MouseEvent::Type::Release;
			mouseEvent.button = spk::Mouse::Button::Left;
			break;

		case WM_RBUTTONUP:
			mouseEvent.type = MouseEvent::Type::Release;
			mouseEvent.button = spk::Mouse::Button::Right;
			break;

		case WM_MBUTTONUP:
			mouseEvent.type = MouseEvent::Type::Release;
			mouseEvent.button = spk::Mouse::Button::Middle;
			break;

		case WM_LBUTTONDBLCLK:
			mouseEvent.type = MouseEvent::Type::DoubleClick;
			mouseEvent.button = spk::Mouse::Button::Left;
			break;

		case WM_RBUTTONDBLCLK:
			mouseEvent.type = MouseEvent::Type::DoubleClick;
			mouseEvent.button = spk::Mouse::Button::Right;
			break;

		case WM_MBUTTONDBLCLK:
			mouseEvent.type = MouseEvent::Type::DoubleClick;
			mouseEvent.button = spk::Mouse::Button::Middle;
			break;

		case WM_MOUSEMOVE:
			mouseEvent.type = MouseEvent::Type::Motion;
			mouseEvent.position = spk::Vector2Int{ LOWORD(wParam), HIWORD(lParam) };
			break;

		case WM_MOUSEWHEEL:
			mouseEvent.type = MouseEvent::Type::Wheel;
			mouseEvent.scrollValue = GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
			break;

		case WM_KEYDOWN:
			keyboardEvent.type = KeyboardEvent::Type::Press;
			keyboardEvent.key = static_cast<spk::Keyboard::Key>(wParam);
			break;

		case WM_KEYUP:
			keyboardEvent.type = KeyboardEvent::Type::Release;
			keyboardEvent.key = static_cast<spk::Keyboard::Key>(wParam);
			break;

		case WM_CHAR:
			keyboardEvent.type = KeyboardEvent::Type::Press;
			keyboardEvent.glyph = static_cast<wchar_t>(wParam);
			break;

		case WM_SIZE:
			systemEvent.type = SystemEvent::Type::Resize;
			systemEvent.window = p_window;
			systemEvent.newSize = spk::Vector2Int(LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_SETFOCUS:
			systemEvent.type = SystemEvent::Type::TakeFocus;
			break;

		case WM_KILLFOCUS:
			systemEvent.type = SystemEvent::Type::LoseFocus;
			break;

		case WM_CLOSE:
		case WM_QUIT:
			systemEvent.type = SystemEvent::Type::Quit;
			systemEvent.window = p_window;
			break;

		case WM_MOVE:
			systemEvent.type = SystemEvent::Type::Move;
			systemEvent.window = p_window;
			systemEvent.newPosition = spk::Vector2Int(LOWORD(lParam), HIWORD(lParam));
			break;

		default:
			return false;
		}

		setModifiers(uMsg);

		return true;
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