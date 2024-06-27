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
			_applicationToManage->closeWindow(p_event.window);
			break;
		}
		}
	}

	SystemModule::SystemModule(spk::SafePointer<GraphicalApplication> p_applicationToManage) :
			_applicationToManage(p_applicationToManage)
	{

	}
}