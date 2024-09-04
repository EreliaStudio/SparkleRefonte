#pragma once

#include <GL/glew.h>
#include <GL/gl.h>

#include <vector>
#include <cstring>

#include "structure/graphics/spk_geometry_2D.hpp"

namespace spk::OpenGL
{
	class TextureObject
	{
		friend class SamplerObject;

	public:
		enum class Format
		{
			RGB = GL_RGB,
			RGBA = GL_RGBA,
			BGR = GL_BGR,
			BGRA = GL_BGRA,
			GreyLevel = GL_RED,
			DualChannel = GL_RG,
			Error = GL_NONE
		};

		enum class Filtering
		{
			Nearest = GL_NEAREST,
			Linear = GL_LINEAR
		};

		enum class Wrap
		{
			Repeat = GL_REPEAT,
			ClampToEdge = GL_CLAMP_TO_EDGE,
			ClampToBorder = GL_CLAMP_TO_BORDER
		};

		enum class Mipmap
		{
			Disable,
			Enable
		};

	private:
		GLuint _id;
		bool _needUpload = false;

		std::vector<uint8_t> _datas;
		spk::Vector2UInt _size;
		Format _format;
		Filtering _filtering;
		Wrap _wrap;
		Mipmap _mipmap;

		void _upload()
		{
			if (_needUpload)
			{
				if (_id == 0)
				{
					glGenTextures(1, &_id);
				}

				glBindTexture(GL_TEXTURE_2D, _id);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(_filtering));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(_filtering));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(_wrap));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(_wrap));

				if (_mipmap == Mipmap::Enable)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				}

				glTexImage2D(
					GL_TEXTURE_2D, 0, static_cast<GLint>(_format),
					static_cast<GLsizei>(_size.x), static_cast<GLsizei>(_size.y),
					0, static_cast<GLenum>(_format), GL_UNSIGNED_BYTE, _datas.data());

				if (_mipmap == Mipmap::Enable)
				{
					glGenerateMipmap(GL_TEXTURE_2D);
				}

				glBindTexture(GL_TEXTURE_2D, 0);

				_needUpload = false;
			}
		}

		size_t _getBytesPerPixel(const Format& format) const
		{
			switch (format)
			{
			case Format::GreyLevel: return 1;
			case Format::DualChannel: return 2;
			case Format::RGB: return 3;
			case Format::RGBA: return 4;
			default: return 0; // Error or unsupported format
			}
		}

	public:
		TextureObject() :
			_id(0),
			_size{ 0, 0 },
			_format(Format::Error),
			_filtering(Filtering::Nearest),
			_wrap(Wrap::Repeat),
			_mipmap(Mipmap::Disable)
		{
		}

		virtual ~TextureObject()
		{
			if (_id != 0)
			{
				glDeleteTextures(1, &_id);
			}
		}

		TextureObject(TextureObject&& p_other) noexcept :
			_id(p_other._id),
			_needUpload(p_other._needUpload),
			_datas(std::move(p_other._datas)),
			_size(p_other._size),
			_format(p_other._format),
			_filtering(p_other._filtering),
			_wrap(p_other._wrap),
			_mipmap(p_other._mipmap)
		{
			p_other._id = 0;
		}

		TextureObject& operator = (TextureObject&& p_other) noexcept
		{
			if (this != &p_other)
			{
				if (_id != 0)
				{
					glDeleteTextures(1, &_id);
				}

				_id = p_other._id;
				_needUpload = p_other._needUpload;
				_datas = std::move(p_other._datas);
				_size = p_other._size;
				_format = p_other._format;
				_filtering = p_other._filtering;
				_wrap = p_other._wrap;
				_mipmap = p_other._mipmap;

				p_other._id = 0;
			}
			return *this;
		}

		void setData(const std::vector<uint8_t>& p_data, const spk::Vector2UInt& p_size,
			const Format& p_format, const Filtering& p_filtering,
			const Wrap& p_wrap, const Mipmap& p_mipmap)
		{
			_datas = p_data;
			_size = p_size;
			_format = p_format;
			_filtering = p_filtering;
			_wrap = p_wrap;
			_mipmap = p_mipmap;
			_needUpload = true;
		}

		void setData(const std::vector<uint8_t>& p_data, const spk::Vector2UInt& p_size, const Format& p_format)
		{
			_datas = p_data;
			_size = p_size;
			_format = p_format;
			_needUpload = true;
		}

		void setData(const uint8_t* p_data, const spk::Vector2UInt& p_size,
			const Format& p_format, const Filtering& p_filtering,
			const Wrap& p_wrap, const Mipmap& p_mipmap)
		{
			_datas.assign(p_data, p_data + (p_size.x * p_size.y * _getBytesPerPixel(p_format)));
			_size = p_size;
			_format = p_format;
			_filtering = p_filtering;
			_wrap = p_wrap;
			_mipmap = p_mipmap;
			_needUpload = true;
		}

		void setData(const uint8_t* p_data, const spk::Vector2UInt& p_size, const Format& p_format)
		{
			_datas.assign(p_data, p_data + (p_size.x * p_size.y * _getBytesPerPixel(p_format)));
			_size = p_size;
			_format = p_format;
			_needUpload = true;
		}

		void setProperties(const Filtering& p_filtering,
			const Wrap& p_wrap, const Mipmap& p_mipmap)
		{
			_filtering = p_filtering;
			_wrap = p_wrap;
			_mipmap = p_mipmap;
			_needUpload = true;
		}

		const std::vector<uint8_t>& data() const
		{
			return _datas;
		}

		spk::Vector2UInt size() const
		{
			return _size;
		}

		Format format() const
		{
			return _format;
		}

		Filtering filtering() const
		{
			return _filtering;
		}

		Wrap wrap() const
		{
			return _wrap;
		}

		Mipmap mipmap() const
		{
			return _mipmap;
		}
	};
}
