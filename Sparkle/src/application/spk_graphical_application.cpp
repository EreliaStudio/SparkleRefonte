#include "application/spk_graphical_application.hpp"

namespace spk
{
	GraphicalApplication::GraphicalApplication() :
		spk::Application()
	{
		addPreparationStep([&]() {spk::cout.setPrefix(L"MainThread"); });
	}

	spk::SafePointer<Window> GraphicalApplication::createWindow(const std::wstring& p_title, const spk::Geometry2DInt& p_geometry)
	{
		std::unique_ptr<spk::Window> newWindow = std::make_unique<spk::Window>(p_title, p_geometry);
		
		spk::SafePointer<Window> newWindowSafePointer = newWindow.get();

		addBehavior(p_title + L" - Renderer", [newWindowSafePointer]() {newWindowSafePointer->render(); }).relinquish();
		addBehavior(p_title + L" - Updater", [newWindowSafePointer]() {newWindowSafePointer->update(); }).relinquish();

		_windows[p_title] = std::move(newWindow);

		return (newWindowSafePointer);
	}
}