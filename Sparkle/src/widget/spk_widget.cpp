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

	Widget::Widget(const std::wstring& p_name, spk::SafePointer<Widget> p_parent) :
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

	void Widget::addChild(spk::SafePointer<Widget> p_child)
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

	void Widget::_onPaintEvent(const spk::PaintEvent& p_event)
	{

	}

	void Widget::onPaintEvent(const spk::PaintEvent& p_event)
	{
		if (isActive() == false)
			return;

		if (_needGeometryChange == true)
		{
			updateGeometry();
		}

		_onPaintEvent(p_event);

		for (auto& child : children())
		{
			child->onPaintEvent(p_event);
		}
	}

	void Widget::_onUpdateEvent(const spk::UpdateEvent& p_event)
	{

	}

	void Widget::onUpdateEvent(const spk::UpdateEvent& p_event)
	{
		if (isActive() == false)
			return;

		for (auto& child : children())
		{
			child->onUpdateEvent(p_event);
		}

		_onUpdateEvent(p_event);
	}

	void Widget::_onKeyboardEvent(const spk::KeyboardEvent& p_event)
	{

	}

	void Widget::onKeyboardEvent(const spk::KeyboardEvent& p_event)
	{
		if (isActive() == false)
			return;

		for (auto& child : children())
		{
			child->onKeyboardEvent(p_event);
		}

		_onKeyboardEvent(p_event);
	}

	void Widget::_onMouseEvent(const spk::MouseEvent& p_event)
	{

	}

	void Widget::onMouseEvent(const spk::MouseEvent& p_event)
	{
		if (isActive() == false)
			return;

		for (auto& child : children())
		{
			child->onMouseEvent(p_event);
		}

		_onMouseEvent(p_event);
	}
	
	void Widget::_onControllerEvent(const spk::ControllerEvent& p_event)
	{

	}
	
	void Widget::onControllerEvent(const spk::ControllerEvent& p_event)
	{
		if (isActive() == false)
			return;

		for (auto& child : children())
		{
			child->onControllerEvent(p_event);
		}

		_onControllerEvent(p_event);
	}
}