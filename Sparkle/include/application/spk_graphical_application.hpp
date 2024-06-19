#pragma once

#include "application/spk_application.hpp"

#include "structure/spk_safe_pointer.hpp"
#include "structure/graphical/spk_window.hpp"

#include "widget/spk_widget.hpp"

namespace spk
{
	class GraphicalApplication : public Application
	{
	private:
		std::map<std::wstring, std::unique_ptr<Window>> _windows;

	public:
		GraphicalApplication();

		spk::SafePointer<Window> createWindow(const std::wstring& p_title, const spk::Geometry2DInt& p_geometry);
		void closeWindow(const spk::SafePointer<Window>& p_window);
	};
}