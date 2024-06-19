#include "application/spk_graphical_application.hpp"

namespace spk
{
	GraphicalApplication::GraphicalApplication() :
		spk::Application()
	{

	}

	spk::SafePointer<Window> GraphicalApplication::createWindow(const std::wstring& p_title, const spk::Geometry2DInt& p_geometry)
	{
		std::unique_ptr<spk::Window> newWindow = std::make_unique<spk::Window>(this, p_title, p_geometry);
		
		spk::SafePointer<Window> newWindowSafePointer = newWindow.get();

		_windows[p_title] = std::move(newWindow);

		return (newWindowSafePointer);
	}

	void GraphicalApplication::closeWindow(const spk::SafePointer<Window>& p_window)
	{
		if (_windows.contains(p_window->title()) == true)
		{
			_windows.erase(p_window->title());
		}
	}
}