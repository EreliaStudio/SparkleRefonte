#include "sparkle.hpp"

class TestWidget : public spk::Widget
{
private:
	spk::Pipeline _pipeline;
	spk::Pipeline::Object _object;

	void _onGeometryChange()
	{
		struct Vertex
		{
			spk::Vector2 position;
		};

		std::vector<Vertex> vertices = {
			{
				spk::Vector2(0, 1)
			},
			{
				spk::Vector2(-1, -1)
			},
			{
				spk::Vector2(1, -1)
			}
		};
		std::vector<unsigned int> indexes = {
			0, 1, 2
		};

		_object.inputLayout() << vertices;
		_object.indexes() << indexes;

		_object.inputLayout().validate();
		_object.indexes().validate();
	}

	void _onPaintEvent(const spk::PaintEvent& p_event)
	{
		_object.render();
	}

public:
	TestWidget(spk::SafePointer<Widget> p_parent) :
		spk::Widget(L"TestWidget", p_parent),
		_pipeline(L"shader/shader.out"),
		_object(_pipeline.createObject())
	{

	}
};

int main()
{
	spk::GraphicalApplication app;

	spk::SafePointer<spk::Window> window = app.createWindow(L"Playground", spk::Geometry2D({ 100, 100 }, { 800, 800 }));

	TestWidget testWidget = TestWidget(window->widget());
	testWidget.setGeometry({ 0, 0, 800, 800 });
	testWidget.activate();

	return app.run();
}