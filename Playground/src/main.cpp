#include "playground.hpp"

class TestWidget : public spk::Widget
{
private:
	void _onGeometryChange()
	{

	}

	void _onPaintEvent(const spk::PaintEvent& p_event)
	{

	}

	void _onUpdateEvent(const spk::UpdateEvent& p_event)
	{

	}

	void _onKeyboardEvent(const spk::KeyboardEvent& p_event)
	{

	}

	void _onMouseEvent(const spk::MouseEvent& p_event)
	{

	}
	
	void _onControllerEvent(const spk::ControllerEvent& p_event)
	{

	}

public:
	TestWidget(const std::wstring& p_name) :
		spk::Widget(p_name)
	{

	}

	TestWidget(const std::wstring& p_name, Widget* p_parent) :
		spk::Widget(p_name, p_parent)
	{

	}
};

int main()
{
	spk::GraphicalApplication app;

	spk::SafePointer<spk::Window> firstWindow = app.createWindow(L"FirstWindow", spk::Geometry2DInt(0, 0, 800, 400));
	spk::SafePointer<spk::Window> secondWindow = app.createWindow(L"SecondWindow", spk::Geometry2DInt(0, 200, 400, 800));

	return (app.run());
}