#pragma once

#include <string>
#include <stdexcept> // For runtime_error

#include <GL/glew.h>

#include "structure/graphics/opengl/spk_texture_object.hpp"

namespace spk::OpenGL
{
	class SamplerObject
	{
	private:
		std::string _name;
		std::string _designator;
		size_t _index;
		TextureObject* _texture = nullptr;
		GLint _uniformDestination = -1;

	public:
		SamplerObject(std::string p_name, std::string p_designator, size_t p_index) :
			_name(std::move(p_name)),
			_designator(std::move(p_designator)),
			_index(p_index)
		{

		}

		SamplerObject(const SamplerObject& p_other) = delete;
		SamplerObject& operator =(const SamplerObject& p_other) = delete;

		SamplerObject(SamplerObject&& p_other) noexcept :
			_name(std::move(p_other._name)),
			_designator(std::move(p_other._designator)),
			_index(p_other._index),
			_texture(p_other._texture),
			_uniformDestination(p_other._uniformDestination)
		{
			p_other._texture = nullptr;
			p_other._index = static_cast<size_t>(-1);
			p_other._uniformDestination = -1;
		}

		SamplerObject& operator =(SamplerObject&& p_other) noexcept
		{
			if (this != &p_other)
			{
				_name = std::move(p_other._name);
				_designator = std::move(p_other._designator);
				_index = p_other._index;
				_texture = p_other._texture;
				_uniformDestination = p_other._uniformDestination;

				p_other._texture = nullptr;
				p_other._index = static_cast<size_t>(-1);
				p_other._uniformDestination = -1;
			}
			return *this;
		}

		void bind(TextureObject* p_texture) const
		{
			_texture = p_texture;
		}

		void activate()
		{
			if (_texture == nullptr)
			{
				throw std::runtime_error("Can't activate the SamplerObject [" + _name + "] without a linked TextureObject.");
			}

			if (_uniformDestination == -1)
			{
				GLint prog = 0;
				glGetIntegerv(GL_CURRENT_PROGRAM, &prog);

				glUseProgram(prog);

				_uniformDestination = glGetUniformLocation(prog, _designator.c_str());

				glUniform1i(_uniformDestination, _index);
			}

			glActiveTexture(GL_TEXTURE0 + _index);

			if (_texture->_needUpload)
			{
				_texture->_upload();
				_texture->_needUpload = false;
			}

			glBindTexture(GL_TEXTURE_2D, _texture->_id);
		}
	};
}