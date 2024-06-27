#include "application/spk_graphical_application.hpp"

namespace spk
{
	GraphicalApplication::GraphicalApplication() :
		spk::Application()
	{

	}

	GraphicalApplication::WindowHandler::WindowHandler(GraphicalApplication* p_application, const std::wstring& p_title, const spk::Geometry2DInt& p_geometry) :
		window(p_title, p_geometry),
		updateThreadExecutionContract(p_application->addExecutionStep([&]() {
				mouseModule.treatMessages();
				keyboardModule.treatMessages();
				controllerModule.treatMessages();
				updateModule.treatMessages();
			})),
		renderThreadExecutionContract(p_application->addExecutionStep([&]() {
				window.pullEvents();
				paintModule.treatMessages();
				systemModule.treatMessages();
			})),
		systemModule(p_application)
	{
		window.bindModule(&mouseModule);
		window.bindModule(&keyboardModule);
		window.bindModule(&controllerModule);
		window.bindModule(&updateModule);
		window.bindModule(&paintModule);
		window.bindModule(&systemModule);
	}

	void GraphicalApplication::WindowHandler::stop()
	{
		updateThreadExecutionContract.resign();
		renderThreadExecutionContract.resign();
	}

	spk::SafePointer<Window> GraphicalApplication::createWindow(const std::wstring& p_title, const spk::Geometry2DInt& p_geometry)
	{
		_windowHandlers[p_title] = std::make_unique<WindowHandler>(this, p_title, p_geometry);

		return (&(_windowHandlers[p_title]->window));
	}

	void GraphicalApplication::closeWindow(const spk::SafePointer<Window>& p_window)
	{
		if (_windowHandlers.contains(p_window->title()) == true)
		{
			_windowHandlers[p_window->title()]->stop();

			_windowHandlers.erase(p_window->title());
		}
	}
}