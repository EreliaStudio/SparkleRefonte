#pragma once

#include <GL/glew.h>
#include <GL/gl.h>

namespace spk::OpenGL
{
    class VertexArrayObject
    {
    private:
        GLuint _id = 0;

        void _allocate();
        void _release();

    public:
        VertexArrayObject();
        ~VertexArrayObject();

        void activate();
        void deactivate();
    };
}
