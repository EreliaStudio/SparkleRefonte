#include "structure/graphics/spk_window.hpp"

#include "application/spk_graphical_application.hpp"

#include "spk_debug_macro.hpp"
#include <gl/GL.h>
#include <gl/GLU.h>

namespace spk
{
	void Window::_initialize(const std::function<void(spk::SafePointer<spk::Window>)>& p_onClosureCallback)
	{
		_windowRendererThread.start();
		_windowUpdaterThread.start();
		_controllerInputThread.start();
		_onClosureCallback = p_onClosureCallback;
	}

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

		_controllerInputThread.bind(_hwnd);
		_createOpenGLContext();
		
		ShowWindow(_hwnd, SW_SHOW);
		UpdateWindow(_hwnd);
	}

	void Window::_createOpenGLContext()
	{
		_hdc = GetDC(_hwnd);
		PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,
			32,
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			24,
			8,
			0,
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};

		int pixelFormat = ChoosePixelFormat(_hdc, &pfd);
		if (pixelFormat == 0)
		{
			throw std::runtime_error("Failed to choose pixel format.");
		}

		if (SetPixelFormat(_hdc, pixelFormat, &pfd) == FALSE)
		{
			throw std::runtime_error("Failed to set pixel format.");
		}

		_hglrc = wglCreateContext(_hdc);
		if (_hglrc == nullptr)
		{
			throw std::runtime_error("Failed to create OpenGL context.");
		}

		if (wglMakeCurrent(_hdc, _hglrc) == FALSE)
		{
			throw std::runtime_error("Failed to activate OpenGL context.");
		}
	}

	void Window::_destroyOpenGLContext()
	{
		if (_hglrc)
		{
			wglMakeCurrent(nullptr, nullptr);
			wglDeleteContext(_hglrc);
			_hglrc = nullptr;
		}

		if (_hwnd && _hdc)
		{
			ReleaseDC(_hwnd, _hdc);
			_hdc = nullptr;
		}
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
				paintModule.treatMessages();
				systemModule.treatMessages();
			}).relinquish();
		_windowUpdaterThread.addExecutionStep([&]() {
				mouseModule.treatMessages();
				keyboardModule.treatMessages();
				controllerModule.treatMessages();
				updateModule.treatMessages();
			}).relinquish();

		bindModule(&mouseModule);
		bindModule(&keyboardModule);
		bindModule(&controllerModule);
		bindModule(&updateModule);
		bindModule(&paintModule);
		bindModule(&systemModule);

		mouseModule.linkToWidget(_rootWidget.get());
		keyboardModule.linkToWidget(_rootWidget.get());
		controllerModule.linkToWidget(_rootWidget.get());
		updateModule.linkToWidget(_rootWidget.get());
		paintModule.linkToWidget(_rootWidget.get());

		updateModule.linkToController(&(controllerModule.controller()));
		updateModule.linkToMouse(&(mouseModule.mouse()));
		updateModule.linkToKeyboard(&(keyboardModule.keyboard()));

		_rootWidget->activate();
	}

	void Window::move(const spk::Geometry2DInt::Position& p_newPosition)
	{
		_geometry.anchor = p_newPosition;
		_rootWidget->setGeometry(_geometry);
	}
	
	void Window::resize(const spk::Geometry2DInt::Size& p_newSize)
	{
		_geometry.size = p_newSize;
		_rootWidget->setGeometry(_geometry);
	}

	void Window::close()
	{
		_windowRendererThread.stop();
		_windowUpdaterThread.stop();
		_controllerInputThread.stop();

		if (_hwnd)
		{
			DestroyWindow(_hwnd);
			_hwnd = nullptr;
		}
		_destroyOpenGLContext();

		if (_onClosureCallback != nullptr)
			_onClosureCallback(this);
	}

	void Window::clear()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Window::swap() const
	{
		SwapBuffers(_hdc);
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