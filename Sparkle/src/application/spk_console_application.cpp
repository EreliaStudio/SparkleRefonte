#include "application/spk_console_application.hpp"

namespace spk
{
	ConsoleApplication::CentralWidget::CentralWidget() :
		spk::Widget(L"CentralWidget")
	{
		activate();
	}

	ConsoleApplication::ConsoleApplication() :
		Application(),
		_centralWidget(std::make_unique<CentralWidget>())
	{
		addBehavior([&]() { centralWidget()->update(); }).relinquish();
	}

	spk::SafePointer<Widget> ConsoleApplication::centralWidget() const
	{
		return (_centralWidget.get());
	}

	ConsoleApplication::operator spk::SafePointer<Widget>() const
	{
		return centralWidget();
	}
}