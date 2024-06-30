#include "application/module/spk_controller_module.hpp"

namespace spk
{
	void ControllerModule::_treatEvent(spk::ControllerEvent&& p_event)
	{
		switch (p_event.type)
		{
			case spk::ControllerEvent::Type::Motion:
			{
				spk::cout << "Receiving input motion : " << p_event.joystick.values << std::endl;
				break;
			}
			case spk::ControllerEvent::Type::Press:
			{
				spk::cout << "Receiving button press : " << p_event.button << std::endl;
				break;
			}
			case spk::ControllerEvent::Type::Release:
			{
				spk::cout << "Receiving button release : " << p_event.button << std::endl;
				break;
			}
		}
	}

	spk::ControllerEvent ControllerModule::_convertEventToEventType(spk::Event&& p_event)
	{
		return (p_event.controllerEvent);
	}

	ControllerModule::ControllerModule()
	{

	}
}