#include "application/module/spk_paint_module.hpp"

#include "structure/graphical/spk_window.hpp"

namespace spk
{
	void PaintModule::_treatEvent(spk::PaintEvent&& p_event)
	{
		DEBUG_LINE();

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(p_event.window->_hwnd, &ps);

		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

		EndPaint(p_event.window->_hwnd, &ps);
	}

	PaintModule::PaintModule()
	{

	}
}