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

	struct IEvent
	{
		bool consumed;
		Modifiers modifiers;

		IEvent() : consumed(false) {}
	};

	struct PaintEvent : public IEvent
	{
		static inline std::vector<UINT> EventIDs = { WM_PAINT };
		enum class Type
		{
			Requested
		};
		Type type;
		spk::SafePointer<Window> window;
	};

	struct UpdateEvent : public IEvent
	{
		static inline std::vector<UINT> EventIDs = {};
		enum class Type
		{
			Requested
		};
		Type type;
		long long deltaTime;
		long long time;
	};

	struct MouseEvent : public IEvent
	{
		static inline std::vector<UINT> EventIDs = {
			WM_LBUTTONDOWN, WM_RBUTTONDOWN, WM_MBUTTONDOWN,
			WM_LBUTTONUP, WM_RBUTTONUP, WM_MBUTTONUP,
			WM_LBUTTONDBLCLK, WM_RBUTTONDBLCLK, WM_MBUTTONDBLCLK,
			WM_MOUSEMOVE, WM_MOUSEWHEEL
		};
		enum class Type
		{
			Press,
			Release,
			DoubleClick,
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
	};

	struct KeyboardEvent : public IEvent
	{
		static inline std::vector<UINT> EventIDs = {
			WM_KEYDOWN, WM_KEYUP, WM_CHAR
		};
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
	};

	struct ControllerEvent : public IEvent
	{
		static inline std::vector<UINT> EventIDs = {};
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
	};

	struct SystemEvent : public IEvent
	{
		static inline std::vector<UINT> EventIDs = { 
			WM_SIZE,
			WM_SETFOCUS,
			WM_KILLFOCUS,
			WM_CLOSE,
			WM_QUIT,
			WM_MOVE
		};
		enum class Type
		{
			Resize,
			TakeFocus,
			LoseFocus,
			Quit,
			Move
		};
		Type type;
		union
		{
			struct
			{
				spk::SafePointer<Window> window;
				spk::Vector2Int newSize;
			};
			struct
			{
				spk::SafePointer<Window> window;
				spk::Vector2Int newPosition;
			};
		};
	};

	struct Event
	{
		union
		{
			PaintEvent paintEvent;
			UpdateEvent updateEvent;
			MouseEvent mouseEvent;
			KeyboardEvent keyboardEvent;
			ControllerEvent controllerEvent;
			SystemEvent systemEvent;
		};

		Event();
		Event(spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam);

		bool construct(spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam);
		void setModifiers(UINT uMsg);
	};
}
