#include "application/spk_graphical_application.hpp"

namespace spk
{
	GraphicalApplication::GraphicalApplication()
	{

	}

	spk::SafePointer<Window> GraphicalApplication::createWindow(const std::wstring& p_title, const spk::Geometry2DInt& p_geometry)
	{
		_windows[p_title] = std::make_unique<spk::Window>(p_title, p_geometry);

		_windows[p_title]->_initialize();

		return (_windows[p_title].get());
	}

	void GraphicalApplication::closeWindow(spk::SafePointer<Window> p_windowToClose)
	{
		_windows.erase(p_windowToClose->title());
	}
}