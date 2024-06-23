#include "application/spk_console_application.hpp"

namespace spk
{
	ConsoleApplication::ConsoleApplication() :
		Application(),
		_centralWidget(std::make_unique<Widget>(L"CentralWidget"))
	{
		_centralWidget->activate();
		addExecutionStep([&]() { centralWidget()->update(); }).relinquish();
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