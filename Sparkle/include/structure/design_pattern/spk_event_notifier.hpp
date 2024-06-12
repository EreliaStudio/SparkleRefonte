#pragma once

#include "structure/design_pattern/spk_contract_provider.hpp"
#include <map>

namespace spk
{
	template<typename TType>
	class EventNotifier
	{
	private:
		std::map<TType, ContractProvider> _eventProviders;

	public:
		EventNotifier() = default;
		virtual ~EventNotifier() = default;

		ContractProvider::Contract subscribe(const TType& event, const ContractProvider::Job& job)
		{
			return _eventProviders[event].subscribe(job);
		}

		void invalidateContracts(const TType& event)
		{
			_eventProviders[event].invalidateContracts();
		}

		void unsubscribe(const TType& event, const ContractProvider::Contract& p_contract)
		{
			_eventProviders[event].unsubscribe(p_contract);
		}

		void notifyEvent(const TType& event)
		{
			auto it = _eventProviders.find(event);
			if (it != _eventProviders.end())
			{
				it->second.trigger();
			}
		}
	};
}