#pragma once


#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "structure/spk_safe_pointer.hpp"
#include "structure/graphics/spk_geometry_2D.hpp"

#include "structure/container/spk_thread_safe_queue.hpp"

#include "structure/system/device/spk_controller_input_thread.hpp"


#include "structure/design_pattern/spk_contract_provider.hpp"

#include "widget/spk_widget.hpp"

#include "structure/thread/spk_persistant_worker.hpp"

#include "application/module/spk_module.hpp"

#include <deque>
#include <map>
#include <unordered_set>
#include <Windows.h>

#include "application/module/spk_mouse_module.hpp"
#include "application/module/spk_keyboard_module.hpp"
#include "application/module/spk_system_module.hpp"
#include "application/module/spk_update_module.hpp"
#include "application/module/spk_paint_module.hpp"
#include "application/module/spk_controller_module.hpp"

namespace spk
{
	class Window
	{
		friend struct Event;
		friend class GraphicalApplication;

	private:
		std::unique_ptr<Widget> _rootWidget;
		std::function<void()> _centralWidgetConstructor;
		
		std::wstring _title;
		spk::Geometry2DInt _geometry;
		spk::PersistantWorker _windowRendererThread;
		spk::PersistantWorker _windowUpdaterThread;
		ControllerInputThread _controllerInputThread;

		HWND _hwnd;
		HDC _hdc;
		HGLRC _hglrc;

		MouseModule mouseModule;
		KeyboardModule keyboardModule;
		SystemModule systemModule;
		UpdateModule updateModule;
		PaintModule paintModule;
		ControllerModule controllerModule;

		std::map<UINT, spk::IModule*> _subscribedModules;
		std::function<void(spk::SafePointer<spk::Window>)> _onClosureCallback = nullptr;

		void _initialize(const std::function<void(spk::SafePointer<spk::Window>)>& p_onClosureCallback);
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		void _createContext();
		void _createOpenGLContext();
		void _destroyOpenGLContext();

		bool _receiveEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);

	public:
		Window(const std::wstring& p_title, const spk::Geometry2DInt& p_geometry);
		~Window();

		template <typename TMainWidget, typename... Args>
		void setMainWidget(Args&&... p_args)
		{
			_centralWidgetConstructor = [this, ...forwarded_args = std::forward<Args>(p_args)]() mutable {
					TMainWidget* newWidget = _rootWidget->makeChild<TMainWidget>(std::forward<decltype(forwarded_args)>(forwarded_args)...);
					newWidget->setGeometry(spk::Geometry2DInt({ 0, 0 }, geometry().size));
					newWidget->activate();
					requestPaint();
				};
		}

		void move(const spk::Geometry2DInt::Position& p_newPosition);
		void resize(const spk::Geometry2DInt::Size& p_newSize);
		void close();
		void clear();
		void swap() const;

		void pullEvents();
		void bindModule(spk::IModule* p_module);

		void requestPaint() const;
		void requestUpdate() const;

		spk::SafePointer<Widget> widget() const;
		operator spk::SafePointer<Widget>() const;

		const std::wstring& title() const;
		const spk::Geometry2DInt& geometry() const;
	};
}