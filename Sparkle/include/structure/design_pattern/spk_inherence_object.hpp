#pragma once

#include <vector>
#include <stdexcept>

namespace spk
{
	template <typename TType>
	class InherenceObject
	{
	public:
		using Parent = InherenceObject<TType>*;
		using Child = InherenceObject<TType>*;
		using ChildObject = TType*;
		using ChildArray = std::vector<ChildObject>;

	private:
		Parent _parent;
		ChildArray _children;

	protected:
		InherenceObject() : _parent(nullptr)
		{
		}

	public:
		virtual ~InherenceObject()
		{
			for (Child child : _children)
			{
				child->_parent = nullptr;
			}
		}

		virtual void addChild(ChildObject p_child)
		{
			_children.push_back(p_child);
			static_cast<Child>(p_child)->_parent = static_cast<Child>(this);
		}

		virtual void removeChild(ChildObject p_child)
		{
			auto it = std::find(_children.begin(), _children.end(), p_child);
			if (it == _children.end())
			{
				throw std::runtime_error("Child not found in children array");
			}
			_children.erase(it);
			static_cast<Child>(p_child)->_parent = nullptr;
		}

		void transferChildren(Parent p_newParent)
		{
			for (ChildObject child : _children)
			{
				p_newParent->addChild(child);
			}
			_children.clear();
		}

		Parent parent() const
		{
			return _parent;
		}

		const ChildArray& children() const
		{
			return _children;
		}
	};
}