#include "structure/graphics/opengl/spk_layout_buffer_object.hpp"

namespace spk::OpenGL
{
	LayoutBufferObject::LayoutBufferObject() : VertexBufferObject(Type::Storage, Usage::Static)
	{
	}

	void LayoutBufferObject::addAttribute(GLuint index, Attribute::Type attributeType)
	{
		GLint size;
		GLenum type;

		switch (attributeType)
		{
		case Attribute::Type::Int:
			size = 1;
			type = GL_INT;
			break;
		case Attribute::Type::Float:
			size = 1;
			type = GL_FLOAT;
			break;
		case Attribute::Type::UInt:
			size = 1;
			type = GL_UNSIGNED_INT;
			break;
		case Attribute::Type::Vector2:
			size = 2;
			type = GL_FLOAT;
			break;
		case Attribute::Type::Vector3:
			size = 3;
			type = GL_FLOAT;
			break;
		case Attribute::Type::Vector4:
			size = 4;
			type = GL_FLOAT;
			break;
		case Attribute::Type::Vector2Int:
			size = 2;
			type = GL_INT;
			break;
		case Attribute::Type::Vector3Int:
			size = 3;
			type = GL_INT;
			break;
		case Attribute::Type::Vector4Int:
			size = 4;
			type = GL_INT;
			break;
		default:
			throw std::runtime_error("Unknown attribute type.");
		}

		_vertexSize += size * getTypeSize(type);
		_attributes.emplace_back(index, size, type);

		_isAttributeInitialized = false; // Mark attributes as uninitialized
	}

	void LayoutBufferObject::activate()
	{
		VertexBufferObject::activate();
		if (!_isAttributeInitialized)
		{
			_applyAttributes();
			_isAttributeInitialized = true; // Mark attributes as initialized
		}
	}

	void LayoutBufferObject::_applyAttributes()
	{
		size_t offset = 0;
		for (const auto& attr : _attributes)
		{
			glEnableVertexAttribArray(attr.index);
			glVertexAttribPointer(attr.index, attr.size, attr.type, GL_FALSE, _vertexSize, reinterpret_cast<void*>(offset));
			offset += attr.size * getTypeSize(attr.type);
		}
	}

	size_t LayoutBufferObject::getTypeSize(GLenum type) const
	{
		switch (type)
		{
		case GL_BYTE: return sizeof(GLbyte);
		case GL_UNSIGNED_BYTE: return sizeof(GLubyte);
		case GL_SHORT: return sizeof(GLshort);
		case GL_UNSIGNED_SHORT: return sizeof(GLushort);
		case GL_INT: return sizeof(GLint);
		case GL_UNSIGNED_INT: return sizeof(GLuint);
		case GL_FLOAT: return sizeof(GLfloat);
		case GL_DOUBLE: return sizeof(GLdouble);
		default: throw std::runtime_error("Unknown GL type.");
		}
	}
}
