#include "structure/graphical/spk_window.hpp"

#include "application/spk_graphical_application.hpp"

#include "spk_debug_macro.hpp"

namespace spk
{
	void Window::_initialize()
	{
		_windowRendererThread.start();
		_windowUpdaterThread.start();
	}

	LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		/*Window* window = nullptr;

		if (uMsg == WM_NCCREATE)
		{
			CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
			window = reinterpret_cast<Window*>(pCreate->lpCreateParams);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
			std::cout << "Setting user data for hwnd [" << hwnd << "] : " << window << std::endl;
		}
		else
		{
			window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		}

		if (window != nullptr && window->_receiveEvent(uMsg, wParam, lParam) == true)
			return (0);*/

		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	bool Window::_receiveEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (_subscribedModules.contains(uMsg) == false)
			return (false);
		_subscribedModules[uMsg]->receiveEvent(spk::Event(this, uMsg, wParam, lParam));
		return (true);
	}

	void Window::_createContext()
	{
		_hwnd = CreateWindowEx(
			0,
			L"SPKWindowClass",
			_title.c_str(),
			WS_OVERLAPPEDWINDOW,
			_geometry.x, _geometry.y,
			_geometry.width, _geometry.height,
			nullptr, nullptr, GetModuleHandle(nullptr), this);

		if (!_hwnd)
		{
			throw std::runtime_error("Failed to create window.");
		}

		ShowWindow(_hwnd, SW_SHOW);
		UpdateWindow(_hwnd);

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}


	Window::Window(const std::wstring& p_title, const spk::Geometry2DInt& p_geometry) :
		_rootWidget(std::make_unique<Widget>(p_title + L" - CentralWidget")),
		_title(p_title),
		_geometry(p_geometry),
		_windowRendererThread(p_title + L" - Renderer"),
		_windowUpdaterThread(p_title + L" - Updater")
	{
		_windowRendererThread.addPreparationStep([&]() {_createContext(); }).relinquish();
		_windowRendererThread.addExecutionStep([&]() {
				pullEvents();
				
			}).relinquish();
		_windowUpdaterThread.addExecutionStep([&]() {
				
			}).relinquish();

		bindModule(&mouseModule);
		bindModule(&keyboardModule);
		bindModule(&controllerModule);
		bindModule(&updateModule);
		bindModule(&paintModule);
		bindModule(&systemModule);
	}

	void Window::close()
	{
		_windowRendererThread.stop();
		_windowUpdaterThread.stop();

		if (_hwnd)
		{
			DestroyWindow(_hwnd);
			_hwnd = nullptr;
		}
	}

	void Window::clear()
	{

	}

	void Window::swap()
	{

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
	
	void Window::bindModule(spk::IModule* p_module)
	{
		for (const auto& ID : p_module->eventIDs())
		{
			_subscribedModules[ID] = p_module;
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