#pragma once

#include <vector>
#include <stdexcept>
#include <GL/glew.h>
#include "structure/graphics/opengl/spk_vertex_array_object.hpp"
#include "structure/graphics/opengl/spk_vertex_buffer_object.hpp"

namespace spk::OpenGL
{
    class Storage
    {
    private:
        VertexArrayObject _vao;
        VertexBufferObject _vboInput;
        VertexBufferObject _vboElement;

        size_t _vertexSize = 0;

    public:
        Storage();

        // Method to add an attribute to the input layout
        void addAttribute(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, size_t offset);

        // Method to send vertex data
        template <typename TType>
        void sendVertices(const std::vector<TType>& vertices);

        // Method to send element data (indices)
        void sendIndices(const std::vector<uint32_t>& indices);

        // Method to activate the storage
        void activate();

        // Method to deactivate the storage
        void deactivate();
    };
}

