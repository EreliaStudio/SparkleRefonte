#include "application/spk_graphical_application.hpp"

namespace spk
{
	GraphicalApplication::GraphicalApplication() :
		spk::WidgetBasedApplication()
	{
		addBehavior([&]() { centralWidget()->render(); }).relinquish();
		addBehavior([&]() { centralWidget()->update(); }).relinquish();
	}
}