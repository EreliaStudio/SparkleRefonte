#pragma once

#include "application/spk_application.hpp"
#include "widget/spk_widget.hpp"

namespace spk
{
	class WidgetBasedApplication : public spk::Application
	{
	private:
		class CentralWidget : public spk::Widget
		{
		private:

		public:
			CentralWidget();
		};

		std::unique_ptr<CentralWidget> _centralWidget;

	public:
		WidgetBasedApplication();

		spk::SafePointer<Widget> centralWidget() const;

		operator spk::SafePointer<Widget>() const;
	};
}