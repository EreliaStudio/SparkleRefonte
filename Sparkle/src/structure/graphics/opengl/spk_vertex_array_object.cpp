#include "structure/graphics/opengl/spk_vertex_array_object.hpp"
#include <GL/glew.h>
#include <windows.h>

namespace spk::OpenGL
{
    void VertexArrayObject::_allocate()
    {
        if (wglGetCurrentContext() != nullptr)
        {
            glGenVertexArrays(1, &_id);
        }
    }

    void VertexArrayObject::_release()
    {
        if (wglGetCurrentContext() != nullptr)
            glDeleteVertexArrays(1, &_id);
        _id = 0;
    }

    VertexArrayObject::VertexArrayObject()
    {

    }

    VertexArrayObject::~VertexArrayObject()
    {
        if (_id != 0)
        {
            _release();
        }
    }

    void VertexArrayObject::activate()
    {
        if (_id == 0)
        {
            _allocate();
        }
        glBindVertexArray(_id);
    }

    void VertexArrayObject::deactivate()
    {
        glBindVertexArray(0);
    }
}
