#pragma once

#include "structure/spk_safe_pointer.hpp"
#include "structure/graphical/spk_geometry_2D.hpp"

#include "structure/design_pattern/spk_contract_provider.hpp"

namespace spk
{
	class GraphicalApplication;
	class Widget;

	class Window
	{
	private:
		std::unique_ptr<Widget> _rootWidget;
		std::wstring _title;
		spk::Geometry2DInt _geometry;

	public:
		Window(GraphicalApplication* p_application, const std::wstring& p_title, const spk::Geometry2DInt& p_geometry);

		void clear();
		void render();
		void swap();

		void update();

		void pullEvents();

		spk::SafePointer<Widget> widget() const;
		operator spk::SafePointer<Widget>() const;

		const std::wstring& title() const;
		const spk::Geometry2DInt& geometry() const;
	};
}