#include "application/module/spk_update_module.hpp"

namespace spk
{
	void UpdateModule::_treatEvent(spk::UpdateEvent&& p_event)
	{
		DEBUG_LINE();
	}

	spk::UpdateEvent UpdateModule::_convertEventToEventType(spk::Event&& p_event)
	{
		return (p_event.updateEvent);
	}

	UpdateModule::UpdateModule()
	{

	}
}