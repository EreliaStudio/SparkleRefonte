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
				
			})),
		renderThreadExecutionContract(p_application->addExecutionStep([&]() {
				window.pullEvents();
			}))
	{

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
			_windowHandlers.erase(p_window->title());
		}
	}
}