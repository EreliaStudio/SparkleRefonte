#pragma once

#include <GL/glew.h>
#include <GL/gl.h>

namespace spk::OpenGL
{
    class VertexArrayObject
    {
    private:
        GLuint _id = 0;

        void _allocate()
        {
            if (wglGetCurrentContext() != nullptr)
            {
                glGenVertexArrays(1, &_id);
            }
        }

        void _release()
        {
            if (wglGetCurrentContext() != nullptr)
            {
                glDeleteVertexArrays(1, &_id);
            }
            _id = 0;
        }

    public:
        VertexArrayObject()
        {

        }

        ~VertexArrayObject()
        {
            if (_id != 0)
            {
                _release();
            }
        }

        void activate()
        {
            if (_id == 0)
            {
                _allocate();
            }

            glBindVertexArray(_id);
        }

        void deactivate()
        {
            glBindVertexArray(0);
        }
    };
}
