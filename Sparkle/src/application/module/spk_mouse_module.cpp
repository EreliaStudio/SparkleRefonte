#include "application/module/spk_mouse_module.hpp"

namespace spk
{
	void MouseModule::_treatEvent(spk::MouseEvent&& p_event)
	{
		switch (p_event.type)
		{			
			case spk::MouseEvent::Type::Press:
				spk::cout << "Press click on : " << p_event.button << std::endl;
				break;
			case spk::MouseEvent::Type::Release:
				spk::cout << "Release click on : " << p_event.button << std::endl;
				break;
			case spk::MouseEvent::Type::DoubleClick:
				spk::cout << "Double click on : " << p_event.button << std::endl;
				break;
			case spk::MouseEvent::Type::Motion:
				spk::cout << "Mouse position : " << p_event.position << std::endl;
				break;
			case spk::MouseEvent::Type::Wheel:
				spk::cout << "Wheel position : " << p_event.scrollValue << std::endl;
				break;
		}
	}

	spk::MouseEvent MouseModule::_convertEventToEventType(spk::Event&& p_event)
	{
		return (p_event.mouseEvent);
	}

	MouseModule::MouseModule()
	{

	}
}