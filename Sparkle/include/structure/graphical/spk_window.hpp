#pragma once

#include "structure/spk_safe_pointer.hpp"
#include "structure/graphical/spk_geometry_2D.hpp"

namespace spk
{
	class Widget;

	class Window
	{
	private:
		std::unique_ptr<Widget> _rootWidget;

	public:
		Window(const std::wstring& p_title, const spk::Geometry2DInt& p_geometry);

		void clear();
		void render();
		void swap();

		void update();

		spk::SafePointer<Widget> widget() const;
		operator spk::SafePointer<Widget>() const;
	};
}