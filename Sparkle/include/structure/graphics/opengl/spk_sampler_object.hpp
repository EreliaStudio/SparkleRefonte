#pragma once

#include <string>
#include <stdexcept>
#include <GL/glew.h>
#include "spk_texture_object.hpp"

namespace spk::OpenGL
{
	class SamplerObject
	{
	public:
		class Factory
		{
		private:
			std::string _name;
			std::string _designator;
			size_t _index;

		public:
			Factory() = default;

			void setName(const std::string& name);
			void setDesignator(const std::string& designator);
			void setIndex(size_t index);

			SamplerObject construct() const;
		};

	private:
		std::string _name;
		std::string _designator;
		GLint _index;
		TextureObject* _texture = nullptr;
		GLint _uniformDestination = -1;

	public:
		SamplerObject();
		SamplerObject(const std::string& p_name, const std::string& p_designator, size_t p_index);

		SamplerObject(const SamplerObject& p_other) = delete;
		SamplerObject& operator=(const SamplerObject& p_other) = delete;

		SamplerObject(SamplerObject&& p_other) noexcept;
		SamplerObject& operator=(SamplerObject&& p_other) noexcept;

		void bind(TextureObject* p_texture);
		void activate();
	};
}
