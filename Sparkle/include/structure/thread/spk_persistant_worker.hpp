#pragma once

#include "structure/thread/spk_thread.hpp"

#include "structure/design_pattern/spk_contract_provider.hpp"

namespace spk
{
	class PersistantWorker : public spk::Thread
	{
	public:
		using PreparationJob = spk::ContractProvider::Job;
		using PreparationContract = spk::ContractProvider::Contract;

	private:
		std::atomic<bool> _running;

		spk::ContractProvider _preparationContracts;

		using spk::Thread::join;

	public:
		PersistantWorker(const std::wstring& p_name, const std::function<void()>& p_callback) :
			spk::Thread(p_name, [this, p_callback]()
				{
					this->_running = true;
					_preparationContracts.trigger();
					while (this->_running.load() == true)
					{
						p_callback();
					}
				})
		{

		}

		~PersistantWorker()
		{
			stop();
		}

		void stop()
		{
			this->_running = false;
			if (isJoinable())
			{
				join();
			}
		}

		PreparationContract addPreparationStep(const PreparationJob& p_job)
		{
			return (_preparationContracts.subscribe(p_job));
		}
	};
}