#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <vector>
#include <span>
#include <stdexcept>
#include "structure/graphics/opengl/spk_vertex_buffer_object.hpp"

namespace spk::OpenGL
{
    class IndexBufferObject : public VertexBufferObject
    {
    public:
        IndexBufferObject() :
            VertexBufferObject(Type::Layout, Usage::Static)
        {
        }

        void append(const unsigned int& data)
        {
            VertexBufferObject::append(&data, sizeof(unsigned int));
        }

        void append(const std::vector<unsigned int>& data)
        {
            VertexBufferObject::append(data.data(), data.size() * sizeof(unsigned int));
        }

        void append(const std::span<unsigned int>& data)
        {
            VertexBufferObject::append(data.data(), data.size() * sizeof(unsigned int));
        }

        IndexBufferObject& operator<<(const unsigned int& data)
        {
            append(data);
            return *this;
        }

        IndexBufferObject& operator<<(const std::vector<unsigned int>& data)
        {
            append(data);
            return *this;
        }

        IndexBufferObject& operator<<(const std::span<unsigned int>& data)
        {
            append(data);
            return *this;
        }


        size_t nbIndexes() const
        {
            return size() / sizeof(unsigned int);
        }

        size_t nbTriangles() const
        {
            return nbIndexes() / 3;
        }
    };
}
