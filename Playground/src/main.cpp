#include "sparkle.hpp"

class TestWidget : public spk::Widget
{
private:
	void _onGeometryChange()
	{
		DEBUG_LINE();
	}
	
	void _onPaintEvent(const spk::PaintEvent& p_event)
	{
		DEBUG_LINE();
	}
	
	void _onUpdateEvent(const spk::UpdateEvent& p_event)
	{
		DEBUG_LINE();
	}
	
	void _onKeyboardEvent(const spk::KeyboardEvent& p_event)
	{
		DEBUG_LINE();
	}
	
	void _onMouseEvent(const spk::MouseEvent& p_event)
	{

	}
	
	void _onControllerEvent(const spk::ControllerEvent& p_event)
	{
		DEBUG_LINE();
	}

public:
	TestWidget(spk::SafePointer<Widget> p_parent) :
		spk::Widget(L"TestWidget", p_parent)
	{

	}
};

int main()
{
	spk::GraphicalApplication app;
	
	spk::SafePointer<spk::Window> window = app.createWindow(L"Playground", spk::Geometry2DInt({100, 100}, {800, 800}));

	TestWidget testWidget(window->widget());
	
	testWidget.setGeometry(spk::Geometry2DInt({ 0, 0 }, { 800, 800 }));
	testWidget.activate();

	return app.run();
}

