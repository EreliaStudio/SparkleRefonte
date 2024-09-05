#include "sparkle.hpp"

class TestWidget : public spk::Widget
{
private:
	spk::Pipeline _pipeline;
	spk::Pipeline::Object _object;
	spk::Pipeline::Object::Sampler2D& _sampler;
	spk::Image _texture;

	void _onGeometryChange()
	{
		struct Vertex
		{
			spk::Vector2 position;
			spk::Vector2 uvs;
		};

		std::vector<Vertex> vertices = {
			{
				spk::Vector2(0, 1),
				spk::Vector2(0, 0)
			},
			{
				spk::Vector2(-1, -1),
				spk::Vector2(0, 1)
			},
			{
				spk::Vector2(1, -1),
				spk::Vector2(1, 0)
			}
		};
		std::vector<unsigned int> indexes = {
			0, 1, 2
		};

		_object.layout() << vertices;
		_object.indexes() << indexes;

		_object.layout().validate();
		_object.indexes().validate();

		_sampler.bind(&_texture);
	}

	void _onPaintEvent(const spk::PaintEvent& p_event)
	{
		_object.render();
	}

public:
	TestWidget(spk::SafePointer<Widget> p_parent) :
		spk::Widget(L"TestWidget", p_parent),
		_pipeline(L"shader/shader.lum"),
		_object(_pipeline.createObject()),
		_sampler(_object.sampler2D(L"test::texture")),
		_texture(L"resources/texture/test.png")
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