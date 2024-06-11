#include "widget/spk_widget_tester.hpp"


#include "widget/spk_widget.hpp"

#include "spk_debug_macro.hpp"

namespace 
{
	class CustomWidget : public spk::Widget
	{
	private:
		int _geometryChangeCounter;
		int _renderCounter;
		int _updateCounter;

	protected:
		void _onGeometryChange() override
		{
			_geometryChangeCounter++;
		}

		void _onRender() override
		{
			_renderCounter++;
		}

		void _onUpdate() override
		{
			_updateCounter++;
		}

	public:
		CustomWidget(const spk::String& p_name) :
			Widget(p_name), _geometryChangeCounter(0), _renderCounter(0), _updateCounter(0)
		{

		}

		int geometryChangeCounter() const
		{
			return _geometryChangeCounter;
		}

		int renderCounter() const
		{
			return _renderCounter;
		}

		int updateCounter() const
		{
			return _updateCounter;
		}
	};
}

TEST_F(WidgetTest, GeometryChange)
{
	::CustomWidget widget(L"TestWidget");

	EXPECT_EQ(widget.geometryChangeCounter(), 0) << "Geometry change counter should be 0 at the start of the test";

	spk::Geometry2DInt newGeometry{ 0, 0, 100, 100 };
	widget.setGeometry(newGeometry);
	widget.updateGeometry();

	EXPECT_EQ(widget.geometryChangeCounter(), 1) << "Geometry change counter should be 1 after setting and updating geometry";
}

TEST_F(WidgetTest, RenderInactive)
{
	::CustomWidget widget(L"TestWidget");

	EXPECT_EQ(widget.renderCounter(), 0) << "Render counter should be 0 at the start of the test";

	widget.render();

	EXPECT_EQ(widget.renderCounter(), 0) << "Render counter should still be 0 after calling render() on an inactive widget";
}

TEST_F(WidgetTest, UpdateInactive)
{
	::CustomWidget widget(L"TestWidget");

	EXPECT_EQ(widget.updateCounter(), 0) << "Update counter should be 0 at the start of the test";

	widget.update();

	EXPECT_EQ(widget.updateCounter(), 0) << "Update counter should still be 0 after calling update() on an inactive widget";
}

TEST_F(WidgetTest, RenderActive)
{
	::CustomWidget widget(L"TestWidget");

	EXPECT_EQ(widget.renderCounter(), 0) << "Render counter should be 0 at the start of the test";

	widget.activate(); // Activate the widget before rendering
	widget.render();

	EXPECT_EQ(widget.renderCounter(), 1) << "Render counter should be 1 after calling render() on an active widget";
}

TEST_F(WidgetTest, UpdateActive)
{
	::CustomWidget widget(L"TestWidget");

	EXPECT_EQ(widget.updateCounter(), 0) << "Update counter should be 0 at the start of the test";

	widget.activate(); // Activate the widget before updating
	widget.update();

	EXPECT_EQ(widget.updateCounter(), 1) << "Update counter should be 1 after calling update() on an active widget";
}

TEST_F(WidgetTest, GeometryChangeOnRender)
{
	::CustomWidget widget(L"TestWidget");

	EXPECT_EQ(widget.geometryChangeCounter(), 0) << "Geometry change counter should be 0 at the start of the test";

	spk::Geometry2DInt newGeometry{ 0, 0, 100, 100 };
	widget.setGeometry(newGeometry);

	EXPECT_EQ(widget.geometryChangeCounter(), 0) << "Geometry change counter should still be 0 after setting geometry but before rendering";

	widget.activate();
	widget.render();

	EXPECT_EQ(widget.geometryChangeCounter(), 1) << "Geometry change counter should be 1 after rendering with new geometry";

	widget.render();

	EXPECT_EQ(widget.geometryChangeCounter(), 1) << "Geometry change counter should still be 1 after a second render call";

	widget.updateGeometry();

	EXPECT_EQ(widget.geometryChangeCounter(), 2) << "Geometry change counter should be 2 after a request on geometry change and a second render call";
}

TEST_F(WidgetTest, AddChild)
{
	::CustomWidget parentWidget(L"ParentWidget");
	::CustomWidget childWidget(L"ChildWidget");

	EXPECT_EQ(parentWidget.children().size(), 0) << "Parent widget should have no children initially";
	EXPECT_EQ(parentWidget.depth(), 0) << "Parent widget depth should be 0 when initialized";

	parentWidget.addChild(&childWidget);

	EXPECT_EQ(parentWidget.children().size(), 1) << "Parent widget should have one child after adding a child widget";
	EXPECT_EQ(childWidget.depth(), 1) << "Child widget depth should be 1 after being added to the parent";
}

TEST_F(WidgetTest, SetDepth)
{
	::CustomWidget parentWidget(L"TestWidget");
	::CustomWidget childWidget(L"ChildWidget");

	parentWidget.addChild(&childWidget);

	EXPECT_EQ(parentWidget.depth(), 0) << "Parent depth should be 0 initially";
	EXPECT_EQ(childWidget.depth(), 1) << "Child depth should be 1 initially";

	parentWidget.setDepth(5.0);

	EXPECT_EQ(parentWidget.depth(), 5.0) << "Parent depth should be 5 after setting depth to 5";
	EXPECT_EQ(childWidget.depth(), 6.0) << "Child depth should be 6 after setting parent's depth to 5";
}

TEST_F(WidgetTest, ForceGeometryChange)
{
	::CustomWidget widget(L"TestWidget");

	EXPECT_EQ(widget.geometryChangeCounter(), 0) << "Geometry change counter should be 0 at the start of the test";

	spk::Geometry2DInt newGeometry{ 0, 0, 100, 100 };
	widget.forceGeometryChange(newGeometry);

	EXPECT_EQ(widget.geometryChangeCounter(), 1) << "Geometry change counter should be 1 after forcing geometry change";
}

TEST_F(WidgetTest, RenderWithChildren)
{
	::CustomWidget parentWidget(L"ParentWidget");
	::CustomWidget childWidget1(L"ChildWidget1");
	::CustomWidget childWidget2(L"ChildWidget2");

	parentWidget.addChild(&childWidget1);
	parentWidget.addChild(&childWidget2);

	parentWidget.activate();
	childWidget1.activate();
	childWidget2.activate();

	EXPECT_EQ(parentWidget.renderCounter(), 0) << "Parent render counter should be 0 at the start of the test";
	EXPECT_EQ(childWidget1.renderCounter(), 0) << "Child1 render counter should be 0 at the start of the test";
	EXPECT_EQ(childWidget2.renderCounter(), 0) << "Child2 render counter should be 0 at the start of the test";

	parentWidget.render();

	EXPECT_EQ(parentWidget.renderCounter(), 1) << "Parent render counter should be 1 after calling render()";
	EXPECT_EQ(childWidget1.renderCounter(), 1) << "Child1 render counter should be 1 after parent calling render()";
	EXPECT_EQ(childWidget2.renderCounter(), 1) << "Child2 render counter should be 1 after parent calling render()";
}

TEST_F(WidgetTest, UpdateWithChildren)
{
	::CustomWidget parentWidget(L"ParentWidget");
	::CustomWidget childWidget1(L"ChildWidget1");
	::CustomWidget childWidget2(L"ChildWidget2");

	parentWidget.addChild(&childWidget1);
	parentWidget.addChild(&childWidget2);

	parentWidget.activate();
	childWidget1.activate();
	childWidget2.activate();

	EXPECT_EQ(parentWidget.updateCounter(), 0) << "Parent update counter should be 0 at the start of the test";
	EXPECT_EQ(childWidget1.updateCounter(), 0) << "Child1 update counter should be 0 at the start of the test";
	EXPECT_EQ(childWidget2.updateCounter(), 0) << "Child2 update counter should be 0 at the start of the test";

	parentWidget.update();

	EXPECT_EQ(parentWidget.updateCounter(), 1) << "Parent update counter should be 1 after calling update()";
	EXPECT_EQ(childWidget1.updateCounter(), 1) << "Child1 update counter should be 1 after parent calling update()";
	EXPECT_EQ(childWidget2.updateCounter(), 1) << "Child2 update counter should be 1 after parent calling update()";
}