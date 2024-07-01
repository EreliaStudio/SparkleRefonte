#include "application/module/spk_mouse_module.hpp"

namespace spk
{
	void MouseModule::_treatEvent(spk::MouseEvent&& p_event)
	{
		_rootWidget->onMouseEvent(p_event);
	}

	spk::MouseEvent MouseModule::_convertEventToEventType(spk::Event&& p_event)
	{
		return (p_event.mouseEvent);
	}

	MouseModule::MouseModule()
	{

	}

	void MouseModule::linkToWidget(spk::Widget* p_rootWidget)
	{
		_rootWidget = p_rootWidget;
	}
}