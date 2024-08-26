#include "structure/graphics/opengl/spk_element_buffer_object.hpp"

namespace spk::OpenGL
{
    ElementBufferObject::ElementBufferObject() :
        VertexBufferObject(Type::Layout, Usage::Static)
    {
    }

    void ElementBufferObject::append(const unsigned int& data)
    {
        VertexBufferObject::append(&data, sizeof(unsigned int));
    }

    void ElementBufferObject::append(const std::vector<unsigned int>& data)
    {
        VertexBufferObject::append(data.data(), data.size() * sizeof(unsigned int));
    }

    void ElementBufferObject::append(const std::span<unsigned int>& data)
    {
        VertexBufferObject::append(data.data(), data.size() * sizeof(unsigned int));
    }

    ElementBufferObject& ElementBufferObject::operator<<(const unsigned int& data)
    {
        append(data);
        return *this;
    }

    ElementBufferObject& ElementBufferObject::operator<<(const std::vector<unsigned int>& data)
    {
        append(data);
        return *this;
    }

    ElementBufferObject& ElementBufferObject::operator<<(const std::span<unsigned int>& data)
    {
        append(data);
        return *this;
    }
}
