#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <vector>
#include <span>
#include <stdexcept>
#include "structure/graphics/opengl/spk_vertex_buffer_object.hpp"

namespace spk::OpenGL
{
    class ElementBufferObject : private VertexBufferObject
    {
    public:
        ElementBufferObject();

        using VertexBufferObject::activate;
        using VertexBufferObject::deactivate;
        using VertexBufferObject::validate;

        void append(const unsigned int& data);
        void append(const std::vector<unsigned int>& data);
        void append(const std::span<unsigned int>& data);

        ElementBufferObject& operator<<(const unsigned int& data);
        ElementBufferObject& operator<<(const std::vector<unsigned int>& data);
        ElementBufferObject& operator<<(const std::span<unsigned int>& data);
    };
}
