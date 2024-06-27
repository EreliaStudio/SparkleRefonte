#pragma once

#include "application/module/spk_module.hpp"

#include "structure/system/event/spk_event.hpp"

#include "spk_debug_macro.hpp"

namespace spk
{
	class ControllerModule : public spk::Module<spk::ControllerEvent>
	{
	private:
		void _treatEvent(spk::ControllerEvent&& p_event);

	public:
		ControllerModule();
	};
}