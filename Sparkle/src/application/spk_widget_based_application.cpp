#include "application/spk_widget_based_application.hpp"

namespace spk
{
	WidgetBasedApplication::CentralWidget::CentralWidget() :
		spk::Widget(L"CentralWidget")
	{
		activate();
	}

	WidgetBasedApplication::WidgetBasedApplication() :
		Application(),
		_centralWidget(std::make_unique<CentralWidget>())
	{

	}

	spk::SafePointer<Widget> WidgetBasedApplication::centralWidget() const
	{
		return (_centralWidget.get());
	}
	
	WidgetBasedApplication::operator spk::SafePointer<Widget>() const
	{
		return centralWidget();
	}
}