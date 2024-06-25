#pragma once

#include "structure/container/spk_thread_safe_queue.hpp"
#include "structure/system/event/spk_event.hpp"

namespace spk
{
	class Module
	{
	private:
		spk::ThreadSafeQueue<spk::Event> eventQueue;

		virtual void _treatEvent(spk::Event&& p_event) = 0;

	public:
		Module()
		{

		}

		void treatMessages()
		{
			while (eventQueue.empty() == false)
			{
				_treatEvent(std::move(eventQueue.pop()));
			}
		}

		spk::ThreadSafeQueue<spk::Event>& eventQueue()
		{
			return (eventQueue);
		}
	};
}