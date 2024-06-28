#pragma once

#include "application/module/spk_module.hpp"

#include "structure/system/event/spk_event.hpp"

#include "spk_debug_macro.hpp"

namespace spk
{
	class PaintModule : public spk::Module<spk::PaintEvent>
	{
	private:
		void _treatEvent(spk::PaintEvent&& p_event);

	public:
		PaintModule();
	};
}