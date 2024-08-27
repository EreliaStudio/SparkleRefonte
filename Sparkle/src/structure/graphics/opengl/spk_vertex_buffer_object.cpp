#include "structure/graphics/opengl/spk_vertex_buffer_object.hpp"
#include <GL/glew.h>
#include <windows.h>

#include "spk_debug_macro.hpp"
#include "utils/spk_opengl_utils.hpp"

namespace spk::OpenGL
{
    void VertexBufferObject::_allocate()
    {
        if (wglGetCurrentContext() != nullptr)
        {
            glGenBuffers(1, &_id);
        }
    }

    void VertexBufferObject::_release()
    {
        if (wglGetCurrentContext() != nullptr)
        {
            glDeleteBuffers(1, &_id);
        }
        _id = 0;
    }

    void VertexBufferObject::_update()
    {
        size_t size = _buffer.size();

        if (size > _currentBufferMaxSize)
        {
            glBufferData(static_cast<GLenum>(_type), size, _buffer.data(), static_cast<GLenum>(_usage));
            _currentBufferMaxSize = size;
        }
        else
        {
            glBufferSubData(static_cast<GLenum>(_type), 0, size, _buffer.data());
        }
    }

    VertexBufferObject::VertexBufferObject()
    {
    }

    VertexBufferObject::VertexBufferObject(Type p_type, Usage p_usage) :
        _type(p_type),
        _usage(p_usage)
    {
    }

    VertexBufferObject::~VertexBufferObject()
    {
        if (_id != 0)
        {
            _release();
        }
    }

    VertexBufferObject::VertexBufferObject(const VertexBufferObject& p_other) :
        _type(p_other._type),
        _usage(p_other._usage)
    {
        edit(p_other._buffer.data(), p_other._buffer.size());
        validate();
    }

    VertexBufferObject::VertexBufferObject(VertexBufferObject&& p_other) noexcept :
        _validated(p_other._validated),
        _id(p_other._id),
        _type(p_other._type),
        _usage(p_other._usage),
        _buffer(std::move(p_other._buffer)),
        _currentBufferMaxSize(p_other._currentBufferMaxSize)
    {
        p_other._validated = false;
        p_other._id = 0;
        p_other._type = Type::Unknow;
        p_other._usage = Usage::Unknow;
        p_other._buffer.clear();
        p_other._currentBufferMaxSize = 0;
    }

    VertexBufferObject& VertexBufferObject::operator=(const VertexBufferObject& p_other)
    {
        if (this != &p_other)
        {
            if (_id != 0)
            {
                _release();
            }

            _type = p_other._type;
            _usage = p_other._usage;
            _buffer = p_other._buffer;
            _currentBufferMaxSize = p_other._currentBufferMaxSize;
            _validated = p_other._validated;

            edit(p_other._buffer.data(), p_other._buffer.size());
            validate();
        }

        return *this;
    }

    VertexBufferObject& VertexBufferObject::operator=(VertexBufferObject&& p_other) noexcept
    {
        if (this != &p_other)
        {
            if (_id != 0)
            {
                _release();
            }

            _validated = p_other._validated;
            _id = p_other._id;
            _type = p_other._type;
            _usage = p_other._usage;
            _buffer = std::move(p_other._buffer);
            _currentBufferMaxSize = p_other._currentBufferMaxSize;

            p_other._validated = false;
            p_other._id = 0;
            p_other._type = Type::Unknow;
            p_other._usage = Usage::Unknow;
            p_other._buffer.clear();
            p_other._currentBufferMaxSize = 0;
        }

        return *this;
    }

    void VertexBufferObject::activate()
    {
        if (_id == 0)
        {
            _allocate();
        }
        
        glBindBuffer(static_cast<GLenum>(_type), _id);  // Bind the buffer for usage
        if (_validated)
        {    
            _update();
        }

    }

    void VertexBufferObject::deactivate()
    {
        glBindBuffer(static_cast<GLenum>(_type), 0);  // Unbind the buffer
    }

    void VertexBufferObject::resize(size_t p_size)
    {
        _buffer.resize(p_size);
    }

    void* VertexBufferObject::data()
    {
        return _buffer.data();
    }

    size_t VertexBufferObject::size()
    {
        return _buffer.size();
    }

    void VertexBufferObject::edit(const void* p_data, size_t p_dataSize, size_t p_offset)
    {
        if (_buffer.size() <= p_dataSize + p_offset)
            _buffer.resize(p_dataSize + p_offset);
        _buffer.edit(p_offset, p_data, p_dataSize);
    }

    void VertexBufferObject::append(const void* p_data, size_t p_dataSize)
    {
        _buffer.append(p_data, p_dataSize);
    }

    void VertexBufferObject::validate()
    {
        _validated = true;
    }
}
