#include "application/module/spk_paint_module.hpp"

#include "structure/graphical/spk_window.hpp"

namespace spk
{
	void PaintModule::_treatEvent(spk::PaintEvent&& p_event)
	{

	}

	spk::PaintEvent PaintModule::_convertEventToEventType(spk::Event&& p_event)
	{
		return (p_event.paintEvent);
	}

	PaintModule::PaintModule()
	{

	}
}