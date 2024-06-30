#include "application/spk_graphical_application.hpp"
#include <locale>
#include <codecvt>

namespace spk
{
	std::string wstring_to_string(const std::wstring& wstr)
	{
		if (wstr.empty())
		{
			return std::string();
		}

		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
		std::string str(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str[0], size_needed, NULL, NULL);
		return str;
	}

	GraphicalApplication::GraphicalApplication()
	{
		WNDCLASSEX wc = { 0 };
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = Window::WindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = sizeof(Window*);
		wc.hInstance = GetModuleHandle(nullptr);
		wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wc.lpszMenuName = nullptr;
		wc.lpszClassName = L"SPKWindowClass";
		wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

		WNDCLASSEX existingClass = { 0 };
		if (!GetClassInfoEx(GetModuleHandle(nullptr), L"SPKWindowClass", &existingClass))
		{
			if (!RegisterClassEx(&wc))
			{
				DWORD error = GetLastError();
				std::cerr << "RegisterClass failed with error: " << error << std::endl;
				throw std::runtime_error("Failed to register window class.");
			}
		}

		this->addExecutionStep([&](){
			while (_windowToRemove.empty() == false)
			{
				closeWindow(_windowToRemove.pop());
				if (_windows.size() == 0)
					quit(0);
			}
		}).relinquish();
	}

	spk::SafePointer<Window> GraphicalApplication::createWindow(const std::wstring& p_title, const spk::Geometry2DInt& p_geometry)
	{
		DEBUG_LINE();
		if (_windows.contains(p_title) == true)
			throw std::runtime_error("Can't create a second window named [" + wstring_to_string(p_title) + "]");
		DEBUG_LINE();
		_windows[p_title] = std::make_unique<spk::Window>(p_title, p_geometry);

		DEBUG_LINE();
		_windows[p_title]->_initialize([&](spk::SafePointer<spk::Window> windowPtr){_windowToRemove.push(std::move(windowPtr));});

		DEBUG_LINE();
		return (_windows[p_title].get());
	}

	void GraphicalApplication::closeWindow(spk::SafePointer<Window> p_windowToClose)
	{
		_windows.erase(p_windowToClose->title());
	}
}