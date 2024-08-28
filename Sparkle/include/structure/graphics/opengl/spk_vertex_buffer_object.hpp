#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include "structure/container/spk_data_buffer.hpp"

namespace spk::OpenGL
{
    class VertexBufferObject
    {
    public:
        enum class Type : GLenum
        {
            Unknow = GL_INVALID_ENUM,
            Storage = GL_ARRAY_BUFFER,
            Layout = GL_ELEMENT_ARRAY_BUFFER,
            Uniform = GL_UNIFORM_BUFFER,
            Texture = GL_TEXTURE_BUFFER,
            TransformFeedback = GL_TRANSFORM_FEEDBACK_BUFFER,
            ShaderStorage = GL_SHADER_STORAGE_BUFFER,
            PixelPack = GL_PIXEL_PACK_BUFFER,
            PixelUnpack = GL_PIXEL_UNPACK_BUFFER,
            DrawIndirect = GL_DRAW_INDIRECT_BUFFER,
            AtomicCounter = GL_ATOMIC_COUNTER_BUFFER
        };

        enum class Usage : GLenum
        {
            Unknow = GL_INVALID_ENUM,
            Static = GL_STATIC_DRAW,
            Dynamic = GL_DYNAMIC_DRAW,
            Stream = GL_STREAM_DRAW,
            StaticRead = GL_STATIC_READ,
            DynamicRead = GL_DYNAMIC_READ,
            StreamRead = GL_STREAM_READ,
            StaticCopy = GL_STATIC_COPY,
            DynamicCopy = GL_DYNAMIC_COPY,
            StreamCopy = GL_STREAM_COPY
        };

    protected:
        GLuint _id = 0;

    private:
        bool _validated = false;

        Type _type = Type::Unknow;
        Usage _usage = Usage::Unknow;

        spk::DataBuffer _buffer;
        size_t _currentBufferMaxSize = 0;

        void _allocate()
        {
            if (wglGetCurrentContext() != nullptr)
            {
                glGenBuffers(1, &_id);
            }
        }

        void _release()
        {
            if (wglGetCurrentContext() != nullptr)
            {
                glDeleteBuffers(1, &_id);
            }
            _id = 0;
        }

        void _update()
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

    public:
        VertexBufferObject()
        {
        }

        VertexBufferObject(Type p_type, Usage p_usage) :
            _type(p_type),
            _usage(p_usage)
        {
        }

        ~VertexBufferObject()
        {
            if (_id != 0)
            {
                _release();
            }
        }

        VertexBufferObject(const VertexBufferObject& p_other) :
            _type(p_other._type),
            _usage(p_other._usage)
        {
            edit(p_other._buffer.data(), p_other._buffer.size(), 0);
            validate();
        }

        VertexBufferObject(VertexBufferObject&& p_other) noexcept :
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

        VertexBufferObject& operator=(const VertexBufferObject& p_other)
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

                edit(p_other._buffer.data(), p_other._buffer.size(), 0);
                validate();
            }

            return *this;
        }

        VertexBufferObject& operator=(VertexBufferObject&& p_other) noexcept
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

        virtual void activate()
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

        void deactivate()
        {
            glBindBuffer(static_cast<GLenum>(_type), 0);  // Unbind the buffer
        }

        void resize(size_t p_size)
        {
            _buffer.resize(p_size);
        }

        void* data()
        {
            return _buffer.data();
        }

        size_t size() const
        {
            return _buffer.size();
        }

        void edit(const void* p_data, size_t p_dataSize, size_t p_offset)
        {
            if (_buffer.size() <= p_dataSize + p_offset)
                _buffer.resize(p_dataSize + p_offset);
            _buffer.edit(p_offset, p_data, p_dataSize);
        }

        void append(const void* p_data, size_t p_dataSize)
        {
            _buffer.append(p_data, p_dataSize);
        }

        void validate()
        {
            _validated = true;
        }
    };
}
