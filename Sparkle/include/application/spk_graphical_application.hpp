#pragma once

#include "application/spk_application.hpp"

#include "structure/spk_safe_pointer.hpp"
#include "structure/graphical/spk_window.hpp"

#include "widget/spk_widget.hpp"

#include "application/module/spk_mouse_module.hpp"
#include "application/module/spk_keyboard_module.hpp"
#include "application/module/spk_system_module.hpp"
#include "application/module/spk_update_module.hpp"
#include "application/module/spk_paint_module.hpp"
#include "application/module/spk_controller_module.hpp"

namespace spk
{
	class GraphicalApplication : public Application
	{
	private:
		struct WindowHandler
		{
			Window window;

			Application::Contract updateThreadExecutionContract;
			Application::Contract renderThreadExecutionContract;

			MouseModule mouseModule;
			KeyboardModule keyboardModule;
			SystemModule systemModule;
			UpdateModule updateModule;
			PaintModule paintModule;
			ControllerModule controllerModule;

			WindowHandler(GraphicalApplication* p_application, const std::wstring& p_title, const spk::Geometry2DInt& p_geometry);
			void stop();
		};

		std::map < std::wstring, std::unique_ptr<WindowHandler>> _windowHandlers;

	public:
		GraphicalApplication();

		spk::SafePointer<Window> createWindow(const std::wstring& p_title, const spk::Geometry2DInt& p_geometry);
		void closeWindow(const spk::SafePointer<Window>& p_window);
	};
}