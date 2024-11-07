#pragma once

#include "spk_vertex_array_object.hpp"
#include "spk_index_buffer_object.hpp"
#include "spk_layout_buffer_object.hpp"

namespace spk::OpenGL
{
	class BufferSet
	{
	public:
		enum class Direction
		{
			In,
			Out
		};

		class Factory
		{
		private:
			OpenGL::LayoutBufferObject::Factory _inputLayoutFactory;
			OpenGL::LayoutBufferObject::Factory _outputLayoutFactory;

		public:
			void insert(Direction p_direction, OpenGL::LayoutBufferObject::Attribute::Index p_index, size_t p_size, OpenGL::LayoutBufferObject::Attribute::Type p_type);
			BufferSet construct(Direction p_direction) const;
		};

	private:
		VertexArrayObject _vao;
		LayoutBufferObject _layout;
		IndexBufferObject _indexes;

	public:
		BufferSet();
		LayoutBufferObject& layout();
		IndexBufferObject& indexes();

		const LayoutBufferObject& layout() const;
		const IndexBufferObject& indexes() const;

		void activate();
		void deactivate();
	};
}
