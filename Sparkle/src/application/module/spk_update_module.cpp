#include "application/module/spk_update_module.hpp"

#include "structure/graphical/spk_window.hpp"

namespace spk
{
	void UpdateModule::_treatEvent(spk::UpdateEvent&& p_event)
	{
		_rootWidget->onUpdateEvent(p_event);
	}

	spk::UpdateEvent UpdateModule::_convertEventToEventType(spk::Event&& p_event)
	{
		return (p_event.updateEvent);
	}

	UpdateModule::UpdateModule()
	{

	}

	void UpdateModule::linkToWidget(spk::Widget* p_rootWidget)
	{
		_rootWidget = p_rootWidget;
	}
}