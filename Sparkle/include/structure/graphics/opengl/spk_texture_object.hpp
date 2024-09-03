#pragma once

#include <GL/glew.h>
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

		std::vector<char> _datas;
		Size _size;
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
					static_cast<GLsizei>(_size.width), static_cast<GLsizei>(_size.height),
					0, static_cast<GLenum>(_format), GL_UNSIGNED_BYTE, _datas.data());

				if (_mipmap == Mipmap::Enable)
				{
					glGenerateMipmap(GL_TEXTURE_2D);
				}

				glBindTexture(GL_TEXTURE_2D, 0);

				_needUpload = false;
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

		TextureObject(const uint8_t* p_textureData)
		{
			_id = 0;
			_needUpload = false;
			_size = { 0, 0 };
			_format = Format::Error;
			_filtering = Filtering::Nearest;
			_wrap = Wrap::Repeat;
			_mipmap = Mipmap::Disable;

			setData(std::vector<char>(p_textureData, p_textureData + std::strlen(reinterpret_cast<const char*>(p_textureData))));
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

		void setData(std::vector<char>&& p_data, const Size& p_size,
			const Format& p_format, const Filtering& p_filtering,
			const Wrap& p_wrap, const Mipmap& p_mipmap)
		{
			_datas = std::move(p_data);
			_size = p_size;
			_format = p_format;
			_filtering = p_filtering;
			_wrap = p_wrap;
			_mipmap = p_mipmap;
			_needUpload = true;
		}

		void setData(std::vector<char>&& p_data)
		{
			_datas = std::move(p_data);
			_needUpload = true;
		}

		void resize(const Size& p_size)
		{
			_size = p_size;
			_needUpload = true;
		}

		void setFormat(const Format& p_format)
		{
			_format = p_format;
			_needUpload = true;
		}

		void setFiltering(const Filtering& p_filtering)
		{
			_filtering = p_filtering;
			_needUpload = true;
		}

		void setWrap(const Wrap& p_wrap)
		{
			_wrap = p_wrap;
			_needUpload = true;
		}

		void setMipmap(const Mipmap& p_mipmap)
		{
			_mipmap = p_mipmap;
			_needUpload = true;
		}

		std::vector<char>& data()
		{
			return _datas;
		}

		Size size() const
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
