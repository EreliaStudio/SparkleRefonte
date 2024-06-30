#include "application/module/spk_mouse_module.hpp"

namespace spk
{
	void MouseModule::_treatEvent(spk::MouseEvent&& p_event)
	{

	}

	spk::MouseEvent MouseModule::_convertEventToEventType(spk::Event&& p_event)
	{
		return (p_event.mouseEvent);
	}

	MouseModule::MouseModule()
	{

	}
}