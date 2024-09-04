#pragma once

#include <unordered_map>
#include <regex>
#include <string>


#include "structure/graphics/opengl/spk_uniform_buffer_object.hpp"
#include "structure/graphics/opengl/spk_buffer_set.hpp"
#include "structure/graphics/opengl/spk_sampler_object.hpp"


namespace spk
{
	class ShaderParser
	{
	private:
		struct ConstantInformation
		{
			spk::OpenGL::UniformBufferObject::Factory factory;
			int bindingPoint;
			size_t size;

			ConstantInformation() = default;
			ConstantInformation(const spk::OpenGL::UniformBufferObject::Factory& factory, int bindingPoint, size_t size)
				: factory(factory), bindingPoint(bindingPoint), size(size) {}
		};

		static inline const std::string LAYOUTS_DELIMITER = "## LAYOUTS DEFINITION ##";
		static inline const std::string CONSTANTS_DELIMITER = "## CONSTANTS DEFINITION ##";
		static inline const std::string ATTRIBUTES_DELIMITER = "## ATTRIBUTES DEFINITION ##";
		static inline const std::string TEXTURES_DELIMITER = "## TEXTURES DEFINITION ##";
		static inline const std::string VERTEX_DELIMITER = "## VERTEX SHADER CODE ##";
		static inline const std::string FRAGMENT_DELIMITER = "## FRAGMENT SHADER CODE ##";

		std::string _vertexShaderCode;
		std::string _fragmentShaderCode;

		static inline std::unordered_map<std::wstring, ConstantInformation> _constantInfoMap;
		std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject::Factory> _currentConstantFactories;
		std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject::Factory> _attributeFactories;
		std::unordered_map<std::wstring, spk::OpenGL::SamplerObject::Factory> _textureFactories;
		spk::OpenGL::BufferSet::Factory _objectBufferFactory;

		std::string _getFileSection(const std::string& p_inputCode, const std::string& p_delimiter)
		{
			auto startPos = p_inputCode.find(p_delimiter);
			if (startPos == std::string::npos)
			{
				throw std::runtime_error("Delimiter [" + p_delimiter + "] not found in input code.");
			}

			startPos += p_delimiter.length() + 1;

			auto endPos = p_inputCode.find("##", startPos);
			if (endPos == std::string::npos)
			{
				endPos = p_inputCode.length();
			}

			return p_inputCode.substr(startPos, endPos - startPos);
		}

		std::string _cleanAndCondenseWhitespace(const std::string& input)
		{
			std::string result = input;

			std::replace(result.begin(), result.end(), '\t', ' ');
			std::replace(result.begin(), result.end(), '\n', ' ');

			std::regex multipleSpaces("\\s+");
			result = std::regex_replace(result, multipleSpaces, " ");

			result = StringUtils::trimWhitespace(result);

			return result;
		}

		void _parseUniformLayout(spk::OpenGL::UniformBufferObject::Factory& p_factory, spk::OpenGL::UniformBufferObject::Layout& p_layoutToFeed, const std::vector<std::string>& p_words, size_t& p_index)
		{
			std::wstring name = StringUtils::stringToWString(p_words[p_index]);
			size_t cpuOffset = std::stoi(p_words[p_index + 1]);
			size_t cpuSize = std::stoi(p_words[p_index + 2]);
			size_t gpuOffset = std::stoi(p_words[p_index + 3]);
			size_t gpuSize = std::stoi(p_words[p_index + 4]);
			spk::OpenGL::UniformBufferObject::Layout& newLayout = p_factory.addInnerLayout(p_layoutToFeed, name, { cpuOffset, cpuSize }, { gpuOffset, gpuSize });
			p_index += 5;
			if (p_words[p_index] == "{")
			{
				p_index++;
				while (p_words[p_index] != "}")
				{
					_parseUniformLayout(p_factory, newLayout, p_words, p_index);
				}
				p_index++;
			}
			else if (p_words[p_index] == "{}")
			{
				p_index++;
			}
		}

		std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject::Factory> _parseUniformDescriptors(const std::string& p_constantDescriptors)
		{
			std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject::Factory> result;

			std::string section = StringUtils::mergeWhitespace(p_constantDescriptors);

			std::vector<std::string> words = StringUtils::splitString(section, " ");

			if (words.size() > 2)
			{
				for (size_t i = 0; i < words.size(); i++)
				{
					std::string uniformType = words[i];
					std::string uniformName = words[i + 1];

					spk::OpenGL::UniformBufferObject::Factory newFactory;

					newFactory.setTypeName(words[i]);
					newFactory.mainLayout()._cpu.size = std::stoi(words[i + 2]);
					newFactory.mainLayout()._gpu.size = std::stoi(words[i + 3]);

					i += 5;
					while (words[i] != "}")
					{
						_parseUniformLayout(newFactory, newFactory.mainLayout(), words, i);
					}

					result[StringUtils::stringToWString(uniformName)] = newFactory;
				}
			}

			return result;
		}

		void _assignBindingPointsToConstants()
		{
			for (auto& [key, factory] : _currentConstantFactories)
			{
				std::wstring typeName = StringUtils::stringToWString(factory.typeName());
				if (_constantInfoMap.find(typeName) == _constantInfoMap.end())
				{
					int bindingPoint = static_cast<int>(_constantInfoMap.size());
					size_t size = factory.mainLayout()._cpu.size;

					_constantInfoMap[typeName] = ConstantInformation(factory, bindingPoint, size);

					factory.setBindingPoint(bindingPoint);
				}
				else
				{
					const ConstantInformation& existingInfo = _constantInfoMap[typeName];
					if (existingInfo.size != factory.mainLayout()._cpu.size)
					{
						throw std::runtime_error("Constant [" + factory.typeName() + "] already parsed with a different size");
					}

					factory.setBindingPoint(existingInfo.bindingPoint);
				}
			}
		}

		void _assignBindingPointsToAttributes()
		{
			size_t offset = 0;
			for (auto& [key, factory] : _attributeFactories)
			{
				factory.setBindingPoint(static_cast<int>(_constantInfoMap.size() + offset));
				offset++;
			}
		}

		void _applyBindingPoints(const std::string& type, const std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject::Factory>& factories)
		{
			for (const auto& [key, factory] : factories)
			{
				std::string typeName = factory.typeName();
				int bindingPoint = factory.bindingPoint();

				std::regex pattern("layout\\(" + type + "\\) uniform " + typeName + "\\b");
				std::string replacement = "layout(std140, binding = " + std::to_string(bindingPoint) + ") uniform " + typeName;

				_vertexShaderCode = std::regex_replace(_vertexShaderCode, pattern, replacement);
				_fragmentShaderCode = std::regex_replace(_fragmentShaderCode, pattern, replacement);
			}
		}

		spk::OpenGL::BufferSet::Factory _parseLayoutDescriptors(const std::string& p_layoutDescriptors)
		{
			spk::OpenGL::BufferSet::Factory result;

			std::string section = StringUtils::mergeWhitespace(p_layoutDescriptors);

			std::vector<std::string> words = StringUtils::splitString(section, " ");

			for (size_t i = 0; i < words.size(); i += 3)
			{
				GLuint index = static_cast<GLuint>(std::stoi(words[i]));
				size_t size = static_cast<size_t>(std::stoi(words[i + 1]));

				OpenGL::LayoutBufferObject::Attribute::Type type = static_cast<OpenGL::LayoutBufferObject::Attribute::Type>(std::stoi(words[i + 2]));

				result.insert(index, size, type);
			}

			return (result);
		}

		std::unordered_map<std::wstring, spk::OpenGL::SamplerObject::Factory> _parseTextureDescriptors(const std::string& p_textureDescriptors)
		{
			std::unordered_map<std::wstring, spk::OpenGL::SamplerObject::Factory> result;
			std::vector<std::string> lines = StringUtils::splitString(p_textureDescriptors, " ");

			if (lines.size() > 1)
			{
				for (size_t i = 0; i < lines.size(); i += 3)
				{
					std::string name = lines[i];
					std::string designator = lines[i + 1];
					size_t index = std::stoi(lines[i + 2]);

					spk::OpenGL::SamplerObject::Factory factory;
					factory.setName(name);
					factory.setDesignator(designator);
					factory.setIndex(index);

					result[StringUtils::stringToWString(name)] = factory;
				}
			}

			return result;
		}

	public:
		ShaderParser(const std::string& p_fileContent)
		{
			_vertexShaderCode = _getFileSection(p_fileContent, ShaderParser::VERTEX_DELIMITER);
			_fragmentShaderCode = _getFileSection(p_fileContent, ShaderParser::FRAGMENT_DELIMITER);

			std::string layoutDescriptors = _getFileSection(p_fileContent, ShaderParser::LAYOUTS_DELIMITER);
			_objectBufferFactory = _parseLayoutDescriptors(layoutDescriptors);

			std::string textureDescriptors = _getFileSection(p_fileContent, ShaderParser::TEXTURES_DELIMITER);
			_textureFactories = _parseTextureDescriptors(textureDescriptors);

			std::string constantDescriptors = _getFileSection(p_fileContent, ShaderParser::CONSTANTS_DELIMITER);
			_currentConstantFactories = _parseUniformDescriptors(constantDescriptors);
			_assignBindingPointsToConstants();

			std::string attributeDescriptor = _getFileSection(p_fileContent, ShaderParser::ATTRIBUTES_DELIMITER);
			_attributeFactories = _parseUniformDescriptors(attributeDescriptor);
			_assignBindingPointsToAttributes();

			_applyBindingPoints("constant", _currentConstantFactories);
			_applyBindingPoints("attribute", _attributeFactories);
		}

		static const std::unordered_map<std::wstring, ConstantInformation>& getConstantInfoMap()
		{
			return _constantInfoMap;
		}

		const std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject::Factory>& getCurrentConstantFactories() const
		{
			return _currentConstantFactories;
		}

		const std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject::Factory>& getAttributeFactories() const
		{
			return _attributeFactories;
		}

		const spk::OpenGL::BufferSet::Factory& getLayoutFactory() const
		{
			return _objectBufferFactory;
		}

		const std::unordered_map<std::wstring, spk::OpenGL::SamplerObject::Factory>& getTextureFactories() const
		{
			return _textureFactories;
		}

		const std::string& getVertexShaderCode() const
		{
			return _vertexShaderCode;
		}

		const std::string& getFragmentShaderCode() const
		{
			return _fragmentShaderCode;
		}
	};
}