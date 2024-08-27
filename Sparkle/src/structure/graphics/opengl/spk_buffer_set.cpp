#include "structure/graphics/opengl/spk_buffer_set.hpp"

#include "spk_debug_macro.hpp"

namespace spk::OpenGL
{
    BufferSet::BufferSet()
    {

    }

    LayoutBufferObject& BufferSet::layout()
    {
        return _layout;
    }

    ElementBufferObject& BufferSet::elements()
    {
        return _elements;
    }

    const LayoutBufferObject& BufferSet::layout() const
    {
        return _layout;
    }

    const ElementBufferObject& BufferSet::elements() const
    {
        return _elements;
    }

    void BufferSet::activate()
    {
        _vao.activate();
        _layout.activate();
        _elements.activate();
    }

    void BufferSet::deactivate()
    {
        _vao.deactivate();
        _layout.deactivate();
        _elements.deactivate();
    }
}