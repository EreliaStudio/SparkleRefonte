#pragma once

#include <filesystem>
#include <unordered_map>

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
			OpenGL::BufferSet _inputBufferSet;
			OpenGL::BufferSet _outputBufferSet;
			std::unordered_map<std::wstring, Attribute> _attributes;
			std::unordered_map<std::wstring, Sampler2D> _samplers;

			void _activate();
			void _deactivate();

		public:
			Object() = default;

			OpenGL::LayoutBufferObject& inputLayout();
			OpenGL::LayoutBufferObject& outputLayout();
			OpenGL::IndexBufferObject& indexes();
			Attribute& attribute(const std::wstring& p_name);
			Sampler2D& sampler2D(const std::wstring& p_name);
			void render();
		};

	private:
		ShaderParser _parser;
		spk::OpenGL::Program _program;

		void _render(Object& p_object);

	public:
		Pipeline(const std::filesystem::path& p_shaderFile);

		Object createObject();
	};
}
