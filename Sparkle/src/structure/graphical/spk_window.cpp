#include "structure/graphical/spk_window.hpp"

#include "widget/spk_widget.hpp"
#include "structure/spk_iostream.hpp"

namespace spk
{
	Window::Window(const std::wstring& p_title, const spk::Geometry2DInt& p_geometry) :
		_rootWidget(std::make_unique<Widget>(p_title + L" - CentralWidget"))
	{
		spk::cout << L"Creating window [" << p_title << L"]" << std::endl;
	}

	void Window::clear()
	{

	}

	void Window::render()
	{
		spk::cout << "Rendering process" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		clear();

		_rootWidget->render();

		swap();
	}

	void Window::swap()
	{
		_rootWidget->update();
	}

	void Window::update()
	{
		spk::cout << "Updating process" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	spk::SafePointer<Widget> Window::widget() const
	{
		return (_rootWidget.get());
	}
	
	Window::operator spk::SafePointer<Widget>() const
	{
		return (_rootWidget.get());
	}
}