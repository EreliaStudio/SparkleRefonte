#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <vector>
#include <span>
#include <stdexcept>
#include <string>
#include "structure/container/spk_data_buffer.hpp"
#include "structure/graphics/opengl/spk_vertex_buffer_object.hpp"

namespace spk::OpenGL
{
    class LayoutBufferObject : public VertexBufferObject
    {
    public:
        class Attribute
        {
        public:
            enum class Type
            {
                Int,
                Float,
                UInt,
                Vector2,
                Vector3,
                Vector4,
                Vector2Int,
                Vector3Int,
                Vector4Int
            };

        private:
            GLuint index;
            GLint size;
            GLenum type;

        public:
            Attribute(GLuint p_index, GLint p_size, GLenum p_type);
            friend class LayoutBufferObject; // Allows LayoutBufferObject to access private members
        };

    private:
        std::vector<Attribute> _attributes;
        size_t _vertexSize = 0; // Expected vertex size
        bool _isAttributeInitialized = false; // Tracks whether attributes have been initialized

    public:
        LayoutBufferObject();
        void addAttribute(GLuint index, Attribute::Type attributeType);

        virtual void activate() override;

        template<typename TType>
        void append(const TType& data)
        {
            // Check the size of TType
            checkVertexSize<TType>();
            VertexBufferObject::append(&data, sizeof(TType));
        }

        template<typename TType>
        void append(const std::vector<TType>& data)
        {
            // Check the size of TType
            checkVertexSize<TType>();
            VertexBufferObject::append(data.data(), data.size() * sizeof(TType));
        }

        template<typename TType>
        void append(const std::span<TType>& data)
        {
            // Check the size of TType
            checkVertexSize<TType>();
            VertexBufferObject::append(data.data(), data.size() * sizeof(TType));
        }

        template<typename TType>
        LayoutBufferObject& operator<<(const TType& data)
        {
            append(data);
            return *this;
        }

        template<typename TType>
        LayoutBufferObject& operator<<(const std::vector<TType>& data)
        {
            append(data);
            return *this;
        }

        template<typename TType>
        LayoutBufferObject& operator<<(const std::span<TType>& data)
        {
            append(data);
            return *this;
        }

    private:
        // Private method to apply attributes
        void _applyAttributes();

        // Utility function to get the size of the OpenGL type
        size_t getTypeSize(GLenum type) const;

        // Template function to check the vertex size
        template<typename TType>
        void checkVertexSize() const
        {
            if (sizeof(TType) != _vertexSize)
            {
                throw std::runtime_error("Size mismatch inside LayoutBufferObject: Expected vertex size is " +
                    std::to_string(_vertexSize) + " bytes, but received " +
                    std::to_string(sizeof(TType)) + " bytes.");
            }
        }
    };
}
#pragma once
