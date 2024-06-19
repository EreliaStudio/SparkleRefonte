#pragma once

#include <vector>
#include <map>

#include "structure/design_pattern/spk_contract_provider.hpp"
#include "structure/thread/spk_persistant_worker.hpp"

#include "structure/spk_safe_pointer.hpp"

#include "spk_debug_macro.hpp"

namespace spk
{
	class Application
	{
	public:
		static constexpr const wchar_t* MainThreadName = L"MainThread";
		using Job = spk::ContractProvider::Job;
		using Contract = spk::ContractProvider::Contract;

		using PreparationJob = spk::ContractProvider::Job;
		using PreparationContract = spk::ContractProvider::Contract;

	private:
		std::atomic<bool> _isRunning;
		std::atomic<int> _errorCode;

		std::unordered_map<std::wstring, std::unique_ptr<spk::PersistantWorker>> _workers;
		spk::SafePointer<spk::PersistantWorker> _mainThreadWorker;

	public:
		Application() :
			_mainThreadWorker(worker(MainThreadName))
		{

		}

		spk::SafePointer<spk::PersistantWorker> worker(const std::wstring& p_threadName)
		{
			if (_workers.contains(p_threadName) == false)
				_workers[p_threadName] = std::make_unique<spk::PersistantWorker>(p_threadName);

			return (_workers[p_threadName].get());
		}

		Contract addBehavior(const std::wstring& p_threadName, const Job& p_job)
		{
			return (worker(p_threadName)->addExecutionStep(p_job));
		}

		Contract addBehavior(const Job& p_job)
		{
			return (_workers[MainThreadName]->addExecutionStep(p_job));
		}

		PreparationContract addPreparationStep(const std::wstring& p_threadName, const PreparationJob& p_job)
		{
			return (worker(p_threadName)->addPreparationStep(p_job));
		}

		PreparationContract addPreparationStep(const PreparationJob& p_job)
		{
			return (_workers[MainThreadName]->addPreparationStep(p_job));
		}

		int run()
		{
			_isRunning = true;

			for (auto& [key, worker] : _workers)
			{
				if (key != MainThreadName)
					worker->start();
			}

			spk::cout.setPrefix(L"MainThread");
			_mainThreadWorker->preparationJobs().trigger();
			while (_isRunning == true)
			{
				_mainThreadWorker->executionJobs().trigger();
			}

			for (auto& [key, worker] : _workers)
			{
				if (key != MainThreadName)
					worker->stop();
			}

			return (_errorCode);
		}

		void quit(int p_errorCode)
		{
			_isRunning = false;
			_errorCode = p_errorCode;
		}

		bool isRunning() const
		{
			return (_isRunning);
		}
	};
}