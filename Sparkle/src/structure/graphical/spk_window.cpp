#include "structure/graphical/spk_window.hpp"

#include "widget/spk_widget.hpp"
#include "structure/spk_iostream.hpp"

#include "application/spk_graphical_application.hpp"

#include "structure/system/event/spk_event.hpp"
#include "application/module/spk_module.hpp"

namespace spk
{
	LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		Window* window = nullptr;

		if (uMsg == WM_NCCREATE)
		{
			CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
			window = reinterpret_cast<Window*>(pCreate->lpCreateParams);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
		}
		else
		{
			window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		}

		if (window != nullptr && window->_receiveEvent(uMsg, wParam, lParam) == true)
			return (0);
		
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}


	bool Window::_receiveEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (_subscribedModules.contains(uMsg) == false)
			return (false);
		_subscribedModules[uMsg]->receiveEvent(spk::Event(this, uMsg, wParam, lParam));
		return (true);
	}

	bool Window::_createWindow()
	{
		const wchar_t CLASS_NAME[] = L"Sample Window Class";

		WNDCLASS wc = { };

		wc.lpfnWndProc = WindowProc;
		wc.hInstance = _hInstance;
		wc.lpszClassName = CLASS_NAME;

		RegisterClass(&wc);

		_hwnd = CreateWindowEx(
			0,                              // Optional window styles.
			CLASS_NAME,                     // Window class
			_title.c_str(),                 // Window text
			WS_OVERLAPPEDWINDOW,            // Window style

			// Size and position
			_geometry.x,
			_geometry.y,
			_geometry.width,
			_geometry.height,

			nullptr,       // Parent window    
			nullptr,       // Menu
			_hInstance,    // Instance handle
			this           // Additional application data
		);

		if (_hwnd == nullptr)
		{
			return false;
		}

		return true;
	}

	void Window::_showWindow(int nCmdShow)
	{
		ShowWindow(_hwnd, nCmdShow);
	}

	Window::Window(const std::wstring& p_title, const spk::Geometry2DInt& p_geometry) :
		_rootWidget(std::make_unique<Widget>(p_title + L" - CentralWidget")),
		_title(p_title),
		_geometry(p_geometry),
		_hwnd(nullptr),
		_hInstance(GetModuleHandle(nullptr))
	{
		if (!_createWindow())
		{
			throw std::runtime_error("Failed to create window.");
		}
		_showWindow(SW_SHOW);
	}

	void Window::close()
	{
		SetWindowLongPtr(_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(nullptr));
		_subscribedModules.clear();
	}

	void Window::clear()
	{

	}

	void Window::swap()
	{

	}

	void Window::bindModule(spk::IModule* p_module)
	{
		for (const auto& ID : p_module->eventIDs())
		{
			_subscribedModules[ID] = p_module;
		}
	}

	void Window::pullEvents()
	{
		MSG msg;

		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	spk::SafePointer<Widget> Window::widget() const
	{
		return (_rootWidget.get());
	}
	
	Window::operator spk::SafePointer<Widget>() const
	{
		return (_rootWidget.get());
	}

	const std::wstring& Window::title() const
	{
		return (_title);
	}
	
	const spk::Geometry2DInt& Window::geometry() const
	{
		return (_geometry);
	}
}