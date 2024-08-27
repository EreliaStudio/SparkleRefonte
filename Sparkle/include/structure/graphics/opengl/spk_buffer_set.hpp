#pragma once

#include "structure/graphics/opengl/spk_vertex_array_object.hpp"
#include "structure/graphics/opengl/spk_element_buffer_object.hpp"
#include "structure/graphics/opengl/spk_layout_buffer_object.hpp"


namespace spk::OpenGL
{
    class BufferSet
    {
    private:
        VertexArrayObject _vao;
        LayoutBufferObject _layout;
        ElementBufferObject _elements;

    public:
        BufferSet();

        LayoutBufferObject& layout();
        ElementBufferObject& elements();

        const LayoutBufferObject& layout() const;
        const ElementBufferObject& elements() const;

        void activate();
        void deactivate();
    };
}