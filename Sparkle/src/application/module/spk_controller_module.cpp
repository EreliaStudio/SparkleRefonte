#include "application/module/spk_controller_module.hpp"

namespace spk
{
	void ControllerModule::_treatEvent(spk::ControllerEvent&& p_event)
	{
		_rootWidget->onControllerEvent(p_event);
	}

	spk::ControllerEvent ControllerModule::_convertEventToEventType(spk::Event&& p_event)
	{
		return (p_event.controllerEvent);
	}

	ControllerModule::ControllerModule()
	{

	}

	void ControllerModule::linkToWidget(spk::Widget* p_rootWidget)
	{
		_rootWidget = p_rootWidget;
	}
}