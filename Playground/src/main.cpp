#include "playground.hpp"

class TestWidget : public spk::Widget
{
private:
	void _onGeometryChange()
	{

	}

	void _onPaintEvent(const spk::PaintEvent& p_event)
	{
		spk::cout << __FUNCTIONW__ << "::" << p_event.type << std::endl;
	}

	void _onUpdateEvent(const spk::UpdateEvent& p_event)
	{
		spk::cout << __FUNCTIONW__ << "::" << p_event.type << std::endl;
	}

	void _onKeyboardEvent(const spk::KeyboardEvent& p_event)
	{
		spk::cout << __FUNCTIONW__ << "::" << p_event.type << std::endl;
	}

	void _onMouseEvent(const spk::MouseEvent& p_event)
	{
		spk::cout << "Mouse position : " << p_event.mouse->position << std::endl;
	}
	
	void _onControllerEvent(const spk::ControllerEvent& p_event)
	{
		spk::cout << __FUNCTIONW__ << "::" << p_event.type << std::endl;
	}

public:
	TestWidget(const std::wstring& p_name) :
		spk::Widget(p_name)
	{

	}

	TestWidget(const std::wstring& p_name, spk::SafePointer<Widget> p_parent) :
		spk::Widget(p_name, p_parent)
	{

	}
};

int main()
{
	spk::GraphicalApplication app;

	spk::SafePointer<spk::Window> firstWindow = app.createWindow(L"FirstWindow", spk::Geometry2DInt(0, 0, 800, 400));
	spk::SafePointer<spk::Window> secondWindow = app.createWindow(L"SecondWindow", spk::Geometry2DInt(0, 200, 400, 800));

	TestWidget myWidget(L"TestWidget", firstWindow->widget());
	myWidget.activate();

	TestWidget myWidget2(L"TestWidget", secondWindow->widget());
	myWidget2.activate();

	return (app.run());
}