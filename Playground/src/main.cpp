#include "sparkle.hpp"

#include "structure/graphics/opengl/spk_vertex_array_object.hpp"
#include "structure/graphics/opengl/spk_vertex_buffer_object.hpp"
#include "structure/graphics/opengl/spk_uniform_buffer_object.hpp"

class TestWidget : public spk::Widget
{
private:

	void _onGeometryChange()
	{
        
	}
	
	void _onPaintEvent(const spk::PaintEvent& p_event)
	{

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
    
	TestWidget testWidget = TestWidget(window->widget());
	testWidget.setGeometry({0, 0, 800, 800});
	testWidget.activate();

	return app.run();
}

