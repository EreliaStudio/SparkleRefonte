#include "application/spk_graphical_application.hpp"

namespace spk
{
	GraphicalApplication::GraphicalApplication() :
		spk::Application()
	{

	}

	spk::SafePointer<Window> GraphicalApplication::createWindow(const std::wstring& p_title, const spk::Geometry2DInt& p_geometry)
	{
		std::unique_ptr<spk::Window> newWindow = std::make_unique<spk::Window>(this, p_title, p_geometry);
		
		spk::SafePointer<Window> newWindowSafePointer = newWindow.get();
		Contracts contracts = {
		std::make_unique<spk::ContractProvider::Contract>(addBehavior([newWindowSafePointer]()
			{
				newWindowSafePointer->pullEvents();
			})),
		std::make_unique<spk::ContractProvider::Contract>(addBehavior([newWindowSafePointer]()
			{
				newWindowSafePointer->clear();
				newWindowSafePointer->render();
				newWindowSafePointer->swap();
			})),
		std::make_unique<spk::ContractProvider::Contract>(addBehavior(p_title + L"Updater", [newWindowSafePointer]()
			{
				newWindowSafePointer->update();
			}))
		};

		_windows[p_title] = std::make_tuple<std::unique_ptr<Window>, Contracts>(std::move(newWindow), std::move(contracts));

		return (newWindowSafePointer);
	}

	void GraphicalApplication::closeWindow(const spk::SafePointer<Window>& p_window)
	{
		if (_windows.contains(p_window->title()) == true)
		{
			_windows.erase(p_window->title());
		}
	}
}