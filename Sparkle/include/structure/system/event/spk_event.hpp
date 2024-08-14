#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>
#include "structure/spk_safe_pointer.hpp"
#include "structure/system/device/spk_mouse.hpp"
#include "structure/system/device/spk_keyboard.hpp"
#include "structure/system/device/spk_controller.hpp"

#include "structure/graphics/spk_geometry_2D.hpp"

#include <unordered_map>
#include <functional>
#include <chrono>

static const UINT WM_UPDATE = RegisterWindowMessage(L"WM_UPDATE");
static const UINT WM_LEFT_JOYSTICK_MOTION = RegisterWindowMessage(L"WM_LEFT_JOYSTICK_MOTION");
static const UINT WM_RIGHT_JOYSTICK_MOTION = RegisterWindowMessage(L"WM_RIGHT_JOYSTICK_MOTION");
static const UINT WM_LEFT_TRIGGER_MOTION = RegisterWindowMessage(L"WM_LEFT_TRIGGER_MOTION");
static const UINT WM_RIGHT_TRIGGER_MOTION = RegisterWindowMessage(L"WM_RIGHT_TRIGGER_MOTION");
static const UINT WM_DIRECTIONAL_CROSS_MOTION = RegisterWindowMessage(L"WM_DIRECTIONAL_CROSS_MOTION");
static const UINT WM_CONTROLLER_BUTTON_PRESS = RegisterWindowMessage(L"WM_CONTROLLER_BUTTON_PRESS");
static const UINT WM_CONTROLLER_BUTTON_RELEASE = RegisterWindowMessage(L"WM_CONTROLLER_BUTTON_RELEASE");

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
	private:
		HWND _hwnd;

	public:
		bool consumed;
		Modifiers modifiers;

		IEvent(HWND p_hwnd) :
			consumed(false),
			_hwnd(p_hwnd)
		{}

		void requestPaint() const;
		void requestUpdate() const;
	};

	struct PaintEvent : public IEvent
	{
		static inline std::vector<UINT> EventIDs = { WM_PAINT };
		enum class Type
		{
			Requested
		};
		Type type;
		spk::SafePointer<spk::Window> window;

		PaintEvent(HWND p_hwnd) :
			IEvent(p_hwnd)
		{

		}
	};

	struct UpdateEvent : public IEvent
	{
		static inline std::vector<UINT> EventIDs = {};
		enum class Type
		{
			Requested
		};
		Type type;
		long long time;
		const spk::Mouse* mouse = nullptr;
		const spk::Keyboard* keyboard = nullptr;
		const spk::Controller* controller = nullptr;

		UpdateEvent(HWND p_hwnd) :
			IEvent(p_hwnd),
			type(spk::UpdateEvent::Type::Requested),
			time(duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count())
		{

		}
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
		const spk::Mouse* mouse;
		union
		{
			spk::Mouse::Button button;
			spk::Vector2Int position;
			float scrollValue;
		};

		MouseEvent(HWND p_hwnd) :
			IEvent(p_hwnd)
		{

		}
	};

	struct KeyboardEvent : public IEvent
	{
		static inline std::vector<UINT> EventIDs = {
			WM_KEYDOWN, WM_KEYUP, WM_CHAR
		};
		enum class Type
		{
			Press,
			Release,
			Glyph
		};
		Type type;
		const spk::Keyboard* keyboard;
		union
		{
			spk::Keyboard::Key key;
			wchar_t glyph;
		};

		KeyboardEvent(HWND p_hwnd) :
			IEvent(p_hwnd)
		{

		}
	};

	struct ControllerEvent : public IEvent
	{
		static inline std::vector<UINT> EventIDs = {
			WM_LEFT_JOYSTICK_MOTION,
			WM_RIGHT_JOYSTICK_MOTION,
			WM_LEFT_TRIGGER_MOTION,
			WM_RIGHT_TRIGGER_MOTION,
			WM_DIRECTIONAL_CROSS_MOTION,
			WM_CONTROLLER_BUTTON_PRESS,
			WM_CONTROLLER_BUTTON_RELEASE
		};
		enum class Type
		{
			Press,
			Release,
			TriggerMotion,
			JoystickMotion,
			DirectionalCrossMotion
		};
		static Controller::Button apiValueToControllerButton(int value);

		Type type;
		const spk::Controller* controller;
		union
		{
			spk::Controller::Button button;
			struct
			{
				spk::Controller::Joystick::ID id;
				spk::Vector2Int values;
			} joystick;
			struct
			{
				spk::Controller::Trigger::ID id;
				float value;
			} trigger;
			struct
			{
				spk::Vector2Int values;
			} directionalCross;
		};

		ControllerEvent(HWND p_hwnd) :
			IEvent(p_hwnd)
		{

		}
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
				spk::SafePointer<spk::Window> window;
				spk::Geometry2DInt::Size newSize;
			};
			struct
			{
				spk::SafePointer<spk::Window> window;
				spk::Geometry2DInt::Position newPosition;
			};
		};

		SystemEvent(HWND p_hwnd) :
			IEvent(p_hwnd)
		{

		}
	};

	struct Event
	{
		using ConstructorLambda = std::function<void(Event* p_event, spk::SafePointer<Window> p_window, UINT uMsg, WPARAM wParam, LPARAM lParam)>;
		static const std::unordered_map<UINT, ConstructorLambda> _constructionMap;
	
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

std::ostream& operator << (std::ostream& p_os, const spk::MouseEvent::Type& p_type);
std::wostream& operator << (std::wostream& p_os, const spk::MouseEvent::Type& p_type);

std::ostream& operator << (std::ostream& p_os, const spk::KeyboardEvent::Type& p_type);
std::wostream& operator << (std::wostream& p_os, const spk::KeyboardEvent::Type& p_type);

std::ostream& operator << (std::ostream& p_os, const spk::ControllerEvent::Type& p_type);
std::wostream& operator << (std::wostream& p_os, const spk::ControllerEvent::Type& p_type);

std::ostream& operator << (std::ostream& p_os, const spk::SystemEvent::Type& p_type);
std::wostream& operator << (std::wostream& p_os, const spk::SystemEvent::Type& p_type);

std::ostream& operator << (std::ostream& p_os, const spk::PaintEvent::Type& p_type);
std::wostream& operator << (std::wostream& p_os, const spk::PaintEvent::Type& p_type);

std::ostream& operator << (std::ostream& p_os, const spk::UpdateEvent::Type& p_type);
std::wostream& operator << (std::wostream& p_os, const spk::UpdateEvent::Type& p_type);