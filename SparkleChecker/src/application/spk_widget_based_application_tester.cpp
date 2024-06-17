#include "application/spk_widget_based_application_tester.hpp"

#include "spk_debug_macro.hpp"

namespace
{
	class CustomWidget : public spk::Widget
	{
	private:
		spk::Application* _application;
		int _renderCounter;
		int _updateCounter;

	protected:
		void _onRender() override
		{
			++_renderCounter;
		}

		void _onUpdate() override
		{
			++_updateCounter;
			if (_renderCounter >= 100 && _updateCounter >= 100)
			{
				_application->quit(0);
			}
		}

	public:
		CustomWidget(spk::Application* p_application, const std::wstring& p_name, Widget* p_parent = nullptr)
			: spk::Widget(p_name, p_parent), _application(p_application), _renderCounter(0), _updateCounter(0)
		{
		}
	};
}

TEST_F(WidgetBasedApplicationTest, CentralWidgetInitialization)
{
	auto centralWidget = app.centralWidget();
	ASSERT_NE(centralWidget, nullptr) << "Central widget should be initialized.";
	ASSERT_EQ(centralWidget->depth(), 0.0) << "Central widget should have a default depth of 0.0.";
}

TEST_F(WidgetBasedApplicationTest, AddWidget)
{
	spk::Widget* widget = new spk::Widget(L"TestWidget");
	app.centralWidget()->addChild(widget);

	auto& children = app.centralWidget()->children();
	ASSERT_EQ(children.size(), 1) << "Central widget should have one child after adding a widget.";
	ASSERT_EQ(children[0], widget) << "The added widget should be the child of the central widget.";
}

TEST_F(WidgetBasedApplicationTest, WidgetHierarchy)
{
	spk::Widget* parentWidget = new spk::Widget(L"ParentWidget");
	spk::Widget* childWidget = new spk::Widget(L"ChildWidget");

	app.centralWidget()->addChild(parentWidget);
	parentWidget->addChild(childWidget);

	auto& parentChildren = app.centralWidget()->children();
	ASSERT_EQ(parentChildren.size(), 1) << "Central widget should have one child after adding the parent widget.";
	ASSERT_EQ(parentChildren[0], parentWidget) << "The added parent widget should be the child of the central widget.";

	auto& childChildren = parentWidget->children();
	ASSERT_EQ(childChildren.size(), 1) << "Parent widget should have one child after adding the child widget.";
	ASSERT_EQ(childChildren[0], childWidget) << "The added child widget should be the child of the parent widget.";
}

TEST_F(WidgetBasedApplicationTest, WidgetActivation)
{
	spk::Widget* widget = new spk::Widget(L"ActivableWidget");
	app.centralWidget()->addChild(widget);

	widget->activate();
	ASSERT_TRUE(widget->isActive()) << "Widget should be active after activation.";

	widget->deactivate();
	ASSERT_FALSE(widget->isActive()) << "Widget should be inactive after deactivation.";
}

TEST_F(WidgetBasedApplicationTest, AddActivationCallback)
{
	spk::Widget* widget = new spk::Widget(L"ActivableWidget");
	app.centralWidget()->addChild(widget);

	bool activated = false;
	auto activationCallback = [&activated]() { activated = true; };
	auto contract = widget->addActivationCallback(activationCallback);

	widget->activate();
	ASSERT_TRUE(activated) << "Activation callback should be called when widget is activated.";
	ASSERT_TRUE(contract.isValid()) << "Contract should be valid after adding activation callback.";
}

TEST_F(WidgetBasedApplicationTest, CustomWidgetBehavior)
{
	spk::SafePointer<CustomWidget> customWidget = app.centralWidget()->makeChild<CustomWidget>(&app, L"CustomWidget");
	customWidget->activate();

	app.addBehavior([&]() { app.centralWidget()->render(); }).relinquish();
	app.addBehavior([&]() { app.centralWidget()->update(); }).relinquish();

	int errorCode = app.run();
	ASSERT_EQ(errorCode, 0) << "Application should quit with code 0 after 100 updates and 100 renders.";
}