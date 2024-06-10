#pragma once

#include "structure/thread/spk_thread.hpp"

namespace spk
{
	class PersistantWorker : public spk::Thread
	{
	private:
		std::atomic<bool> _running;

		using spk::Thread::join;

	public:
		PersistantWorker(const spk::String& p_name, const std::function<void()>& p_callback) :
			spk::Thread(p_name, [this, p_callback]()
				{
					this->_running = true;
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
	};
}