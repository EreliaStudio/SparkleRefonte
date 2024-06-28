#include "application/spk_graphical_application.hpp"

namespace spk
{
	GraphicalApplication::GraphicalApplication()
	{

	}

	spk::SafePointer<Window> GraphicalApplication::createWindow(const std::wstring& p_title, const spk::Geometry2DInt& p_geometry)
	{
		return (nullptr);
	}

	void GraphicalApplication::closeWindow(spk::SafePointer<Window> p_windowToClose)
	{

	}
}