#pragma once

#include "application/module/spk_module.hpp"

#include "structure/system/event/spk_event.hpp"

#include "spk_debug_macro.hpp"

namespace spk
{
	class UpdateModule : public spk::Module<spk::UpdateEvent>
	{
	private:
		void _treatEvent(spk::UpdateEvent&& p_event);

	public:
		UpdateModule();
	};
}