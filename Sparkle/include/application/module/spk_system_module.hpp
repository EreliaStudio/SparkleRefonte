#pragma once

#include "application/module/spk_module.hpp"

#include "structure/system/event/spk_event.hpp"

#include "spk_debug_macro.hpp"

namespace spk
{
	class GraphicalApplication;
	
	class SystemModule : public spk::Module<spk::SystemEvent>
	{
	private:
		spk::SafePointer<GraphicalApplication> _applicationToManage;
		
		void _treatEvent(spk::SystemEvent&& p_event);

	public:
		SystemModule(spk::SafePointer<GraphicalApplication> p_applicationToManage);
	};
}