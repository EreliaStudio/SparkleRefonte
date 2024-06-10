#include "application/spk_console_application.hpp"

namespace spk
{
	ConsoleApplication::ConsoleApplication() :
		spk::WidgetBasedApplication()
	{
		addBehavior([&]() { centralWidget()->update(); }).relinquish();
	}
}