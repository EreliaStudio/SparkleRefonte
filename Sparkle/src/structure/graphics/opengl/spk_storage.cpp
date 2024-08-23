#include "structure/graphics/opengl/spk_storage.hpp"

namespace spk::OpenGL
{
    Storage::Storage() :
        _vboInput(VertexBufferObject::Type::Storage, VertexBufferObject::Usage::Static),
        _vboElement(VertexBufferObject::Type::Layout, VertexBufferObject::Usage::Static)
    {
    }

    void Storage::addAttribute(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, size_t offset)
    {
        _vao.activate();
        _vboInput.activate();

        // Enable the attribute array
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, size, type, normalized, stride, reinterpret_cast<void*>(offset));

        _vertexSize += size * sizeof(type);

        _vao.deactivate();
    }

    template <typename TType>
    void Storage::sendVertices(const std::vector<TType>& vertices)
    {
        if (sizeof(TType) != _vertexSize)
        {
            throw std::runtime_error("Size mismatch inside Storage : Expected vertex size is " + std::to_string(_vertexSize) +
                " bytes, but received " + std::to_string(sizeof(TType)) + " bytes.");
        }

        _vboInput.edit(vertices.data(), vertices.size() * sizeof(TType));
    }

    void Storage::sendIndices(const std::vector<uint32_t>& indices)
    {
        _vboElement.edit(indices.data(), indices.size() * sizeof(uint32_t));
    }

    void Storage::activate()
    {
        _vao.activate();
        _vboInput.activate();
        _vboElement.activate();
    }

    void Storage::deactivate()
    {
        _vboElement.deactivate();
        _vboInput.deactivate();
        _vao.deactivate();
    }
}
