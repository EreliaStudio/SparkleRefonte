#include "application/module/spk_system_module.hpp"

#include "application/spk_graphical_application.hpp"

namespace spk
{
	void SystemModule::_treatEvent(spk::SystemEvent&& p_event)
	{
		switch (p_event.type)
		{
		case spk::SystemEvent::Type::Quit:
		{
			spk::cout << "Removing window : " << p_event.window << " with title : " << p_event.window->title() << std::endl;
			p_event.window->close();
			break;
		}
		}
	}

	SystemModule::SystemModule()
	{

	}
}