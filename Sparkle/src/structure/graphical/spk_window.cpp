#include "structure/graphical/spk_window.hpp"

#include "widget/spk_widget.hpp"
#include "structure/spk_iostream.hpp"

#include "application/spk_graphical_application.hpp"

namespace spk
{
	Window::Window(GraphicalApplication* p_application, const std::wstring& p_title, const spk::Geometry2DInt& p_geometry) :
		_rootWidget(std::make_unique<Widget>(p_title + L" - CentralWidget")),
		_title(p_title),
		_geometry(p_geometry)
	{

	}

	void Window::clear()
	{

	}

	void Window::render()
	{
		_rootWidget->render();
	}

	void Window::swap()
	{

	}

	void Window::update()
	{
		_rootWidget->update();
	}

	void Window::pullEvents()
	{

	}

	spk::SafePointer<Widget> Window::widget() const
	{
		return (_rootWidget.get());
	}
	
	Window::operator spk::SafePointer<Widget>() const
	{
		return (_rootWidget.get());
	}

	const std::wstring& Window::title() const
	{
		return (_title);
	}
	
	const spk::Geometry2DInt& Window::geometry() const
	{
		return (_geometry);
	}
}