#pragma once

#include <filesystem>

#include "structure/graphics/opengl/spk_shader_parser.hpp"
#include "structure/graphics/opengl/spk_program.hpp"

#include "utils/spk_file_utils.hpp"

namespace spk
{
	class Pipeline
	{
	public:
		using Constant = spk::OpenGL::UniformBufferObject;

		class Object
		{
			friend class Pipeline;

		public:
			using Attribute = spk::OpenGL::UniformBufferObject;
			using Sampler2D = spk::OpenGL::SamplerObject;

		private:
			Pipeline* _owner;
			OpenGL::BufferSet _bufferSet;
			std::unordered_map<std::wstring, Attribute> _attributes;
			std::unordered_map<std::wstring, Sampler2D> _samplers;

			void _activate()
			{
				_bufferSet.activate();

				for (auto& [name, attribute] : _attributes)
				{
					attribute.activate();
				}

				for (auto& [name, sampler] : _samplers)
				{
					sampler.activate();
				}
			}

			void _deactivate()
			{
				_bufferSet.deactivate();

				for (auto& [name, attribute] : _attributes)
				{
					attribute.deactivate();
				}
			}

		public:
			Object() = default;

			OpenGL::LayoutBufferObject& layout()
			{
				return (_bufferSet.layout());
			}

			OpenGL::IndexBufferObject& indexes()
			{
				return (_bufferSet.indexes());
			}

			Attribute& attribute(const std::wstring& p_name)
			{
				if (_attributes.contains(p_name) == false)
				{
					throw std::runtime_error("Attribute [" + StringUtils::wstringToString(p_name) + "] not found inside Object");
				}
				return (_attributes[p_name]);
			}

			Sampler2D& sampler2D(const std::wstring& p_name)
			{
				if (_samplers.contains(p_name) == false)
				{
					throw std::runtime_error("Sampler2D [" + StringUtils::wstringToString(p_name) + "] not found inside Object");
				}
				return (_samplers[p_name]);
			}

			void render()
			{
				if (_owner == nullptr)
				{
					return;
				}

				_owner->_render(*this);
			}
		};

	private:
		ShaderParser _parser;
		spk::OpenGL::Program _program;

		void _render(Object& p_object)
		{
			_program.activate();

			p_object._activate();

			// Pass the number of indices to the Program's render method
			_program.render(static_cast<GLsizei>(p_object.indexes().nbIndexes()));

			p_object._deactivate();

			_program.deactivate();
		}

	public:
		Pipeline(const std::filesystem::path& p_shaderFile) :
			_parser(FileUtils::readFileAsString(p_shaderFile)),
			_program(_parser.getVertexShaderCode(), _parser.getFragmentShaderCode())
		{
			// The _program will be loaded when _render is called
		}

		Object createObject()
		{
			Object result;

			result._owner = this;
			result._bufferSet = _parser.getLayoutFactory().construct();
			for (const auto& [key, factory] : _parser.getAttributeFactories())
			{
				result._attributes[key] = factory.construct();
			}
			for (const auto& [key, factory] : _parser.getSamplerFactories())
			{
				result._samplers[key] = factory.construct();
			}

			return (result);
		}
	};
}