#include "widget/spk_widget.hpp"

#include "spk_debug_macro.hpp"

namespace spk
{

	Widget::Widget(const std::wstring& p_name) :
		_name(p_name),
		_parent(nullptr),
		_depth(0)
	{

	}

	Widget::Widget(const std::wstring& p_name, Widget* p_parent) :
		Widget(p_name)
	{
		if (p_parent != nullptr)
			p_parent->addChild(this);
	}

	Widget::~Widget()
	{
		if (_parent != nullptr)
			_parent->removeChild(this);

		for (auto& child : _managedChildren)
		{
			delete child;
		}
	}

	const std::wstring& Widget::name() const
	{
		return (_name);
	}

	void Widget::addChild(Widget* p_child)
	{
		spk::InherenceObject<Widget>::addChild(p_child);
		p_child->setDepth(depth() + 1);
	}

	void Widget::setDepth(const float& p_depth)
	{
		float delta = p_depth - _depth;
		for (auto& child : children())
		{
			static_cast<Widget*>(child)->setDepth(static_cast<Widget*>(child)->depth() + delta);
		}
		_depth = p_depth;
	}

	const float& Widget::depth() const
	{
		return (_depth);
	}

	void Widget::forceGeometryChange(const Geometry2DInt& p_geometry)
	{
		_geometry = p_geometry;
		updateGeometry();
	}
	
	void Widget::setGeometry(const Geometry2DInt& p_geometry)
	{
		_geometry = p_geometry;
		_needGeometryChange = true;
	}
	
	void Widget::updateGeometry()
	{
		_onGeometryChange();
		_needGeometryChange = false;
	}

	void Widget::requireGeometryUpdate()
	{
		_needGeometryChange = true;
	}

	const Geometry2DInt& Widget::geometry() const
	{
		return (_geometry);
	}

	void Widget::_onGeometryChange()
	{
	}
	
	void Widget::_onRender()
	{

	}

	void Widget::render()
	{
		if (isActive() == false)
			return ;

		if (_needGeometryChange == true)
		{	
			updateGeometry();
		}

		_onRender();

		for (auto& child : children())
		{
			child->render();
		}
	}

	void Widget::_onUpdate()
	{

	}

	void Widget::update()
	{
		if (isActive() == false)
			return;

		for (auto& child : children())
		{
			child->update();
		}

		_onUpdate();
	}
}