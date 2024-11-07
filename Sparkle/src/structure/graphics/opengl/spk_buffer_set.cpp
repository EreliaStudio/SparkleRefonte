#include "structure/graphics/opengl/spk_buffer_set.hpp"

namespace spk::OpenGL
{
	void BufferSet::Factory::insert(Direction p_direction, OpenGL::LayoutBufferObject::Attribute::Index p_index, size_t p_size, OpenGL::LayoutBufferObject::Attribute::Type p_type)
	{
		switch (p_direction)
		{
		case Direction::In:
		{
			_inputLayoutFactory.insert(p_index, p_size, p_type);
			break;
		}
		case Direction::Out:
		{
			_outputLayoutFactory.insert(p_index, p_size, p_type);
			break;
		}
		}
	}

	BufferSet BufferSet::Factory::construct(Direction p_direction) const
	{
		BufferSet result;
		result._layout = (p_direction == Direction::In ? _inputLayoutFactory.construct() : _outputLayoutFactory.construct());
		return result;
	}

	BufferSet::BufferSet() : _vao(), _layout(), _indexes() {}

	LayoutBufferObject& BufferSet::layout()
	{
		return _layout;
	}

	IndexBufferObject& BufferSet::indexes()
	{
		return _indexes;
	}

	const LayoutBufferObject& BufferSet::layout() const
	{
		return _layout;
	}

	const IndexBufferObject& BufferSet::indexes() const
	{
		return _indexes;
	}

	void BufferSet::activate()
	{
		_vao.activate();
		_layout.activate();
		_indexes.activate();
	}

	void BufferSet::deactivate()
	{
		_vao.deactivate();
		_layout.deactivate();
		_indexes.deactivate();
	}
}
