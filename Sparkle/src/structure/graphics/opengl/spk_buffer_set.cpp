#include "structure/graphics/opengl/spk_buffer_set.hpp"

namespace spk::OpenGL
{
    BufferSet::BufferSet()
    {

    }

    LayoutBufferObject& BufferSet::layout()
    {
        return _layout;
    }

    ElementBufferObject& BufferSet::element()
    {
        return _element;
    }

    void BufferSet::activate()
    {
        _vao.activate();
        _layout.activate();
        _element.activate();
    }

    void BufferSet::deactivate()
    {
        _vao.deactivate();
        _layout.deactivate();
        _element.deactivate();
    }
}