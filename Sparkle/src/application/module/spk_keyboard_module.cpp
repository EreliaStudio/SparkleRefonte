#include "application/module/spk_keyboard_module.hpp"

namespace spk
{
	void KeyboardModule::_treatEvent(spk::KeyboardEvent&& p_event)
	{
		switch (p_event.type)
		{
		case KeyboardEvent::Type::Press:
		{
			//spk::cout << "Pressing key [" << p_event.key << "]" << std::endl;
		}
		case  KeyboardEvent::Type::Release:
		{
			//spk::cout << "Releasing key [" << p_event.key << "]" << std::endl;
		}
		case  KeyboardEvent::Type::Glyph:
		{
			spk::cout << "Entering char [" << p_event.glyph << "]" << std::endl;
		}
		}
	}

	spk::KeyboardEvent KeyboardModule::_convertEventToEventType(spk::Event&& p_event)
	{
		return (p_event.keyboardEvent);
	}

	KeyboardModule::KeyboardModule()
	{

	}
}