#pragma once

#include "spk_define.hpp"

#include "structure/spk_safe_pointer.hpp"

#include "structure/design_pattern/spk_inherence_object.hpp"
#include "structure/design_pattern/spk_activable_object.hpp"

#include "structure/math/spk_geometry_2D.hpp"

namespace spk
{
	struct Vector2Int;

	class Widget :
		public spk::InherenceObject<Widget>, //An object responsible to hold relationship between "child" objects and a unique parent
		public spk::ActivableObject //An object who must contain a state as boolean, with a method activate and deactivate. It must also contain a set of callback where users can add callback to activation and deactivation
	{
	private:
		spk::String _name;
		Widget* _parent;

		std::vector<Widget*> _managedChildren;

		bool _needGeometryChange = true;
		Geometry2DInt _geometry;
		float _depth;

		virtual void _onGeometryChange();
		virtual void _onRender();
		virtual void _onUpdate();

	public:
		Widget(const spk::String& p_name);
		Widget(const spk::String& p_name, Widget* p_parent);

		~Widget();

		const spk::String& name() const;

		void addChild(Widget* p_child) override;

		template<typename TChildType, typename... TArgs>
		spk::SafePointer<TChildType> makeChild(TArgs&&... p_args)
		{
			TChildType* newChild = new TChildType(std::forward<TArgs>(p_args)...);

			addChild(newChild);

			_managedChildren.push_back(newChild);

			return (spk::SafePointer<TChildType>(newChild));
		}

		void setDepth(const float& p_depth);
		const float& depth() const;

		void forceGeometryChange(const Geometry2DInt& p_geometry);
		void setGeometry(const Geometry2DInt& p_geometry);
		void updateGeometry();
		void requireGeometryUpdate();

		const Geometry2DInt& geometry() const;

		void render();
		void update();
	};
}