#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <vector>
#include <span>
#include <stdexcept>
#include <string>

#include "utils/spk_string_utils.hpp"
#include "structure/container/spk_data_buffer.hpp"
#include "structure/container/spk_json_object.hpp"
#include "structure/graphics/opengl/spk_vertex_buffer_object.hpp"

namespace spk::OpenGL
{
    class LayoutBufferObject : public VertexBufferObject
    {
    public:
        class Attribute
        {
        public:
            using Index = GLuint;
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
            Index index;
            GLint size;
            GLenum type;

        public:
            Attribute() = default;
            Attribute(Index p_index, GLint p_size, GLenum p_type);
            friend class LayoutBufferObject; // Allows LayoutBufferObject to access private members
        };

        class Factory
        {
        private:
            std::vector<std::tuple<GLuint, std::string, LayoutBufferObject::Attribute::Type>> layoutInfo;

        public:
            void parse(const spk::JSON::Object& layoutJson);
            void apply(LayoutBufferObject* layoutBuffer);
        };

    private:
        std::vector<Attribute> _attributes;
        size_t _vertexSize = 0; // Expected vertex size
        bool _isAttributeInitialized = false; // Tracks whether attributes have been initialized

    public:
        LayoutBufferObject();
        void addAttribute(Attribute::Index index, Attribute::Type attributeType);

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
        void _applyAttributes();

        size_t _typeSize(GLenum type) const;

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

    LayoutBufferObject::Attribute::Type wstringToAttributeType(const std::wstring& p_value);
}
