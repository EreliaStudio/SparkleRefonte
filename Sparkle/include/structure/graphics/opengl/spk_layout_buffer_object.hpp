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
        struct Attribute
        {
            using Index = GLuint;

            enum class Type
            {
				Byte = GL_BYTE,
				UByte = GL_UNSIGNED_BYTE,
                Int = GL_INT,
                Float = GL_FLOAT,
                UInt = GL_UNSIGNED_INT
            };

            Index index = 0;
            size_t size = 0;
			Type type = Type::Float;

			Attribute()
			{

			}

			Attribute(Index p_index, size_t p_size, Type p_type) :
				index(p_index),
				type(p_type),
				size(p_size)
			{

			}

			static size_t typeSize(Type p_type)
			{
				switch (p_type)
				{
				case Attribute::Type::Byte:
					return (sizeof(GLbyte));
				case Attribute::Type::UByte:
					return (sizeof(GLubyte));
				case Attribute::Type::Int:
					return (sizeof(GLint));
				case Attribute::Type::UInt:
					return (sizeof(GLuint));
				case Attribute::Type::Float:
					return (sizeof(GLfloat));
				}
				return (0);
			}
        };

        class Factory
        {
        private:
			std::vector<Attribute> _attributes;

        public:
			void insert(Attribute::Index p_index, size_t p_size, Attribute::Type p_type)
			{
				_attributes.push_back(Attribute(p_index, p_size, p_type));
			}

			LayoutBufferObject construct()
			{
				LayoutBufferObject result;

				for (const auto& attribute : _attributes)
				{
					result._insertAttribute(attribute);
				}

				return (result);
			}
        };

    private:
		friend class Factory;

        std::vector<Attribute> _attributesToApply;
        size_t _vertexSize = 0;

		void _insertAttribute(const Attribute& p_attribute)
		{
			_attributesToApply.push_back(p_attribute);
			_vertexSize += p_attribute.size * Attribute::typeSize(p_attribute.type);
		}

		void _applyAttributes()
		{
			size_t offset = 0;
			for (const auto& attr : _attributesToApply)
			{
				glEnableVertexAttribArray(attr.index);

				switch (attr.type)
				{
				case Attribute::Type::Byte:
				case Attribute::Type::UByte:
				case Attribute::Type::Int:
				case Attribute::Type::UInt:
				{
					glVertexAttribIPointer(attr.index, static_cast<GLint>(attr.size), static_cast<GLenum>(attr.type), static_cast<GLsizei>(_vertexSize), reinterpret_cast<void*>(offset));
					break;
				}
				case Attribute::Type::Float:
				{
					glVertexAttribPointer(attr.index, static_cast<GLint>(attr.size), static_cast<GLenum>(attr.type), GL_FALSE, static_cast<GLsizei>(_vertexSize), reinterpret_cast<void*>(offset));
					break;
				}
				default:
				{
					throw std::runtime_error("Unexpected layout type.");
				}
				}

				offset += attr.size * Attribute::typeSize(attr.type);
			}
			_attributesToApply.clear();
		}

    public:
		LayoutBufferObject() : VertexBufferObject(Type::Storage, Usage::Static)
		{

		}

		LayoutBufferObject(const LayoutBufferObject& p_other) = delete;
		LayoutBufferObject& operator = (const LayoutBufferObject& p_other) = delete;
		
		LayoutBufferObject(LayoutBufferObject&& p_other) noexcept :
			VertexBufferObject(std::move(p_other)),
			_attributesToApply(std::move(p_other._attributesToApply)),
			_vertexSize(p_other._vertexSize)
		{
			p_other._vertexSize = 0;
		}

		LayoutBufferObject& operator = (LayoutBufferObject&& p_other) noexcept
		{
			if (this != &p_other)
			{
				VertexBufferObject::operator=(std::move(p_other));

				_attributesToApply = std::move(p_other._attributesToApply);
				_vertexSize = p_other._vertexSize;

				p_other._vertexSize = 0;
			}
			return *this;
		}

        virtual void activate() override
		{
			VertexBufferObject::activate();
			if (_attributesToApply.empty() == false)
			{
				_applyAttributes();
			}
		}

        template<typename TType>
        LayoutBufferObject& operator<<(const TType& data)
        {
			append(std::span<const TType>(&data, 1));
            return *this;
        }

        template<typename TType>
        LayoutBufferObject& operator<<(const std::vector<TType>& data)
        {
			append(std::span<const TType>(data));
            return *this;
        }

        template<typename TType>
        LayoutBufferObject& operator<<(const std::span<TType>& data)
        {
			append(data);
            return *this;
        }

		template<typename TType>
		void append(const TType& data)
		{
			append(std::span<const TType>(&data, 1));
		}

		template<typename TType>
		void append(const std::vector<TType>& data)
		{
			append(std::span<const TType>(data));
		}

		template<typename TType>
		void append(const std::span<TType>& data)
		{
			if (sizeof(TType) != _vertexSize)
			{
				throw std::runtime_error("Size mismatch inside LayoutBufferObject: Expected vertex size is " +
					std::to_string(_vertexSize) + " bytes, but received " +
					std::to_string(sizeof(TType)) + " bytes.");
			}
			VertexBufferObject::append(data.data(), data.size() * sizeof(TType));
		}
    };
}
