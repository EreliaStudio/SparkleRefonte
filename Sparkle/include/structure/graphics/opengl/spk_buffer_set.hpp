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
        BufferSet();

        LayoutBufferObject& layout();
        IndexBufferObject& indexes();

        const LayoutBufferObject& layout() const;
        const IndexBufferObject& indexes() const;

        void activate();
        void deactivate();
    };
}