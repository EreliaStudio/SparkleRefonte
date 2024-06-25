#pragma once


#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "structure/spk_safe_pointer.hpp"
#include "structure/graphical/spk_geometry_2D.hpp"

#include "structure/container/spk_thread_safe_queue.hpp"

#include "structure/design_pattern/spk_contract_provider.hpp"

#include <deque>
#include <unordered_set>
#include <Windows.h>

namespace spk
{
	class GraphicalApplication;
	class Widget;

	class Window
	{
	private:
		std::unique_ptr<Widget> _rootWidget;
		
		std::wstring _title;
		spk::Geometry2DInt _geometry;
		
		HWND _hwnd;
		HINSTANCE _hInstance;
		static std::unordered_set<UINT> _subscribedEvents;
		std::map<UINT, spk::ThreadSafeQueue<spk::Event>*> _threadSafeQueues;

		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		bool _createWindow();
		void _showWindow(int nCmdShow);
		void _receiveEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);

	public:
		Window(const std::wstring& p_title, const spk::Geometry2DInt& p_geometry);

		void clear();
		void swap();

		void bind(UINT, spk::ThreadSafeQueue<spk::Event>*);
		void pullEvents();

		spk::SafePointer<Widget> widget() const;
		operator spk::SafePointer<Widget>() const;

		const std::wstring& title() const;
		const spk::Geometry2DInt& geometry() const;
	};
}