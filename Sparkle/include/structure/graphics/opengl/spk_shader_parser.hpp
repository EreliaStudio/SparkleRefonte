#pragma once

#include <unordered_map>
#include <regex>
#include <string>
#include "spk_uniform_buffer_object.hpp"
#include "spk_buffer_set.hpp"
#include "spk_sampler_object.hpp"

namespace spk
{
	class ShaderParser
	{
	private:
		struct ConstantInformation
		{
			spk::OpenGL::UniformBufferObject::Factory factory;
			int bindingPoint = -1;
			size_t size = 0;

			ConstantInformation() = default;
			ConstantInformation(const spk::OpenGL::UniformBufferObject::Factory& factory, int bindingPoint, size_t size);
		};

		static const std::string LAYOUTS_DELIMITER;
		static const std::string CONSTANTS_DELIMITER;
		static const std::string ATTRIBUTES_DELIMITER;
		static const std::string TEXTURES_DELIMITER;
		static const std::string VERTEX_DELIMITER;
		static const std::string FRAGMENT_DELIMITER;

		std::string _vertexShaderCode;
		std::string _fragmentShaderCode;

		static inline std::unordered_map<std::wstring, ConstantInformation> _constantInfoMap;
		std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject::Factory> _currentConstantFactories;
		std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject::Factory> _attributeFactories;
		std::unordered_map<std::wstring, spk::OpenGL::SamplerObject::Factory> _samplerFactories;
		spk::OpenGL::BufferSet::Factory _objectBufferFactory;

		std::string _getFileSection(const std::string& p_inputCode, const std::string& p_delimiter);
		std::string _cleanAndCondenseWhitespace(const std::string& input);
		void _parseUniformLayout(spk::OpenGL::UniformBufferObject::Factory& p_factory, spk::OpenGL::UniformBufferObject::Layout& p_layoutToFeed, const std::vector<std::string>& p_words, size_t& p_index);
		std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject::Factory> _parseUniformDescriptors(const std::string& p_constantDescriptors);
		void _assignBindingPointsToConstants();
		void _assignBindingPointsToAttributes();
		void _applyBindingPoints(const std::string& type, const std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject::Factory>& factories);
		spk::OpenGL::BufferSet::Factory _parseLayoutDescriptors(const std::string& p_layoutDescriptors);
		std::unordered_map<std::wstring, spk::OpenGL::SamplerObject::Factory> _parseSamplerDescriptors(const std::string& p_textureDescriptors);

	public:
		ShaderParser(const std::string& p_fileContent);

		static const std::unordered_map<std::wstring, ConstantInformation>& getConstantInfoMap();
		const std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject::Factory>& getCurrentConstantFactories() const;
		const std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject::Factory>& getAttributeFactories() const;
		const spk::OpenGL::BufferSet::Factory& getLayoutFactory() const;
		const std::unordered_map<std::wstring, spk::OpenGL::SamplerObject::Factory>& getSamplerFactories() const;
		const std::string& getVertexShaderCode() const;
		const std::string& getFragmentShaderCode() const;
	};
}
