#include "structure/graphics/opengl/spk_layout_buffer_object.hpp"

namespace spk::OpenGL
{
	LayoutBufferObject::Attribute::Attribute(Index p_index, GLint p_size, GLenum p_type) :
		index(p_index),
		size(p_size),
		type(p_type)
	{

	}
	void LayoutBufferObject::Factory::parse(const spk::JSON::Object& layoutJson)
	{
		const auto& layoutArray = layoutJson.asArray();
		for (const auto& item : layoutArray)
		{
			GLuint location = item->operator[](L"Location").as<long>();
			std::wstring name = item->operator[](L"Name").as<std::wstring>();
			std::wstring typeStr = item->operator[](L"Type").as<std::wstring>();

			LayoutBufferObject::Attribute::Type type = wstringToAttributeType(typeStr);

			layoutInfo.emplace_back(location, spk::StringUtils::wstringToString(name), type);
		}
	}

	void LayoutBufferObject::Factory::apply(LayoutBufferObject* layoutBuffer)
	{
		for (const auto& [location, name, type] : layoutInfo)
		{
			layoutBuffer->addAttribute(location, type);
		}
	}

	LayoutBufferObject::LayoutBufferObject() : VertexBufferObject(Type::Storage, Usage::Static)
	{
	}

	void LayoutBufferObject::addAttribute(Attribute::Index index, Attribute::Type attributeType)
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

		_vertexSize += size * _typeSize(type);
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
			glVertexAttribPointer(attr.index, attr.size, attr.type, GL_FALSE, static_cast<GLsizei>(_vertexSize), reinterpret_cast<void*>(offset));
			offset += attr.size * _typeSize(attr.type);
		}
	}

	size_t LayoutBufferObject::_typeSize(GLenum type) const
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

	LayoutBufferObject::Attribute::Type wstringToAttributeType(const std::wstring& p_value)
	{
		if (p_value == L"int")
			return LayoutBufferObject::Attribute::Type::Int;
		if (p_value == L"float")
			return LayoutBufferObject::Attribute::Type::Float;
		if (p_value == L"uint")
			return LayoutBufferObject::Attribute::Type::UInt;
		if (p_value == L"vec2")
			return LayoutBufferObject::Attribute::Type::Vector2;
		if (p_value == L"vec3")
			return LayoutBufferObject::Attribute::Type::Vector3;
		if (p_value == L"vec4")
			return LayoutBufferObject::Attribute::Type::Vector4;
		if (p_value == L"ivec2")
			return LayoutBufferObject::Attribute::Type::Vector2Int;
		if (p_value == L"ivec3")
			return LayoutBufferObject::Attribute::Type::Vector3Int;
		if (p_value == L"ivec4")
			return LayoutBufferObject::Attribute::Type::Vector4Int;

		throw std::runtime_error("Unknown attribute type: " + spk::StringUtils::wstringToString(p_value));
	}
}
