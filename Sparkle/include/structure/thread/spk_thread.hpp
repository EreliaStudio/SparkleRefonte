#pragma once

#include "spk_define.hpp"
#include "structure/spk_iostream.hpp"

#include <unordered_map>
#include <vector>
#include <functional>
#include <thread>
#include <memory>

namespace spk
{
	class Thread
	{
	private:
		spk::String _name;
		std::function<void()> _callback;
		std::unique_ptr<std::thread> _handle = nullptr;

	public:
		Thread(const spk::String& p_name, const std::function<void()>& p_callback) :
			_name(p_name),
			_callback([p_name, p_callback]() {
			spk::cout.setPrefix(p_name);
			spk::cerr.setPrefix(p_name);
			p_callback();
				})
		{

		}

		~Thread()
		{
			if (isJoinable())
				join();
		}

		virtual void start()
		{
			if (_handle != nullptr)
			{
				join();
			}
			_handle = std::make_unique<std::thread>(_callback);
		}

		virtual void join()
		{
			if (_handle && _handle->joinable())
			{
				_handle->join();
				_handle.reset();
			}
		}

		bool isJoinable() const
		{
			if (_handle == nullptr)
				return (false);
			return (_handle->joinable());
		}
	};
}