#pragma once

#include "structure/graphics/opengl/spk_vertex_array_object.hpp"
#include "structure/graphics/opengl/spk_index_buffer_object.hpp"
#include "structure/graphics/opengl/spk_layout_buffer_object.hpp"


namespace spk::OpenGL
{
    class BufferSet
    {
    private:
        VertexArrayObject _vao;
        LayoutBufferObject _layout;
        IndexBufferObject _indexes;

    public:
        BufferSet()
        {

        }

        LayoutBufferObject& layout()
        {
            return _layout;
        }
        IndexBufferObject& indexes()
        {
            return _indexes;
        }

        const LayoutBufferObject& layout() const
        {
            return _layout;
        }
        const IndexBufferObject& indexes() const
        {
            return _indexes;
        }

        void activate()
        {
            _vao.activate();
            _layout.activate();
            _indexes.activate();
        }
        void deactivate()
        {
            _vao.deactivate();
            _layout.deactivate();
            _indexes.deactivate();
        }
    };
}