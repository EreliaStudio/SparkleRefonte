#include "application/module/spk_keyboard_module.hpp"

namespace spk
{
	void KeyboardModule::_treatEvent(spk::KeyboardEvent&& p_event)
	{
		DEBUG_LINE();
	}

	spk::KeyboardEvent KeyboardModule::_convertEventToEventType(spk::Event&& p_event)
	{
		return (p_event.keyboardEvent);
	}

	KeyboardModule::KeyboardModule()
	{

	}
}