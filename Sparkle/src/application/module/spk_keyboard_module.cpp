#include "application/module/spk_keyboard_module.hpp"

namespace spk
{
	void KeyboardModule::_treatEvent(spk::KeyboardEvent&& p_event)
	{
		_rootWidget->onKeyboardEvent(p_event);
	}

	spk::KeyboardEvent KeyboardModule::_convertEventToEventType(spk::Event&& p_event)
	{
		return (p_event.keyboardEvent);
	}

	KeyboardModule::KeyboardModule()
	{

	}

	void KeyboardModule::linkToWidget(spk::Widget* p_rootWidget)
	{
		_rootWidget = p_rootWidget;
	}
}