#include "application/module/spk_paint_module.hpp"

#include "structure/graphical/spk_window.hpp"

namespace spk
{
	void PaintModule::_treatEvent(spk::PaintEvent&& p_event)
	{
		switch (p_event.type)
		{
		case spk::PaintEvent::Type::Requested:
		{
			p_event.window->render();
			break;
		}
		}
	}

	spk::PaintEvent PaintModule::_convertEventToEventType(spk::Event&& p_event)
	{
		return (p_event.paintEvent);
	}

	PaintModule::PaintModule()
	{

	}
}