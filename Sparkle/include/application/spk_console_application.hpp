#pragma once

#include "application/spk_application.hpp"
#include "widget/spk_widget.hpp"

namespace spk
{
	class ConsoleApplication : public spk::Application
	{
	private:
		std::unique_ptr<Widget> _centralWidget;

	public:
		ConsoleApplication();

		spk::SafePointer<Widget> centralWidget() const;

		operator spk::SafePointer<Widget>() const;
	};
}