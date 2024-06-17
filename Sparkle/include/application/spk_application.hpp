#pragma once

#include <vector>
#include <map>

#include "structure/design_pattern/spk_contract_provider.hpp"
#include "structure/thread/spk_persistant_worker.hpp"

namespace spk
{
	class Application
	{
	public:
		static constexpr const wchar_t* MainThreadName = L"MainThread";
		using Job = spk::ContractProvider::Job;
		using Contract = spk::ContractProvider::Contract;

	private:
		std::atomic<bool> _isRunning;
		std::atomic<int> _errorCode;

		std::shared_ptr<spk::ContractProvider> _mainThreadContract;
		std::unordered_map<std::wstring, std::shared_ptr<spk::PersistantWorker>> _workers;
		std::unordered_map<std::wstring, std::shared_ptr<spk::ContractProvider>> _contractProviders;

	public:
		Application() :
			_mainThreadContract(std::make_shared<spk::ContractProvider>())
		{
			_contractProviders[MainThreadName] = _mainThreadContract;
			_workers[MainThreadName] = nullptr;
		}

		Contract addBehavior(const std::wstring& p_threadName, const Job& p_job)
		{
			if (_workers.contains(p_threadName) == false)
				_contractProviders[p_threadName] = std::make_shared<spk::ContractProvider>();

			std::shared_ptr<spk::ContractProvider> provider = _contractProviders[p_threadName];

			if (_workers.contains(p_threadName) == false)
				_workers[p_threadName] = std::make_shared<spk::PersistantWorker>(p_threadName, [&, provider](){
					provider->trigger();
				});
			return (_contractProviders[p_threadName]->subscribe(p_job));
		}

		Contract addBehavior(const Job& p_job)
		{
			return (_contractProviders[MainThreadName]->subscribe(p_job));
		}

		int run()
		{
			_isRunning = true;

			for (auto& [key, worker] : _workers)
			{
				if (worker != nullptr)
					worker->start();
			}

			while (_isRunning == true)
			{
				_mainThreadContract->trigger();
			}

			for (auto& [key, worker] : _workers)
			{
				if (worker != nullptr)
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