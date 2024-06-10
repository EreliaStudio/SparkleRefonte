#include "structure/design_pattern/spk_activable_object.hpp"

namespace spk
{
	ActivableObject::ActivableObject() :
		StatefulObject<bool>(false)
	{
	}

	ActivableObject::~ActivableObject()
	{
	}

	void ActivableObject::activate()
	{
		StatefulObject<bool>::setState(true);
	}

	void ActivableObject::deactivate()
	{
		StatefulObject<bool>::setState(false);
	}

	bool ActivableObject::isActive() const
	{
		return state();
	}

	ActivableObject::Contract ActivableObject::addActivationCallback(const std::function<void()>& callback)
	{
		return (StatefulObject<bool>::addCallback(true, callback));
	}

	ActivableObject::Contract ActivableObject::addDeactivationCallback(const std::function<void()>& callback)
	{
		return (StatefulObject<bool>::addCallback(false, callback));
	}
}