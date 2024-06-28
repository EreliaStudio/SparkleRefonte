#pragma once

#include "application/spk_application.hpp"

#include "structure/spk_safe_pointer.hpp"
#include "structure/graphical/spk_window.hpp"

namespace spk
{
	class GraphicalApplication : public Application
	{
	private:

	public:
		GraphicalApplication();

		spk::SafePointer<Window> createWindow(const std::wstring& p_title, const spk::Geometry2DInt& p_geometry);
		void closeWindow(spk::SafePointer<Window> p_windowToClose);
	};
}