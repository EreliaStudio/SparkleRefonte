#include "sparkle.hpp"

#include "utils/spk_string_utils.hpp"
#include "utils/spk_file_utils.hpp"

#include <GL/glew.h>
#include <string>
#include <stdexcept>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <regex>

#include "structure/graphics/opengl/spk_uniform_buffer_object.hpp"
#include "structure/graphics/opengl/spk_buffer_set.hpp"

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

		private:
			Pipeline* _owner;
			OpenGL::BufferSet _bufferSet;
			std::unordered_map<std::wstring, Attribute> _attributes;

			void _activate()
			{
				_bufferSet.activate();

				for (auto& [name, attribute] : _attributes)
				{
					attribute.activate();
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
		static inline const std::string LAYOUTS_DELIMITER = "## LAYOUTS DEFINITION ##";
		static inline const std::string CONSTANTS_DELIMITER = "## CONSTANTS DEFINITION ##";
		static inline const std::string ATTRIBUTES_DELIMITER = "## ATTRIBUTES DEFINITION ##";
		static inline const std::string VERTEX_DELIMITER = "## VERTEX SHADER CODE ##";
		static inline const std::string FRAGMENT_DELIMITER = "## FRAGMENT SHADER CODE ##";

		std::string _vertexShaderCode;
		std::string _fragmentShaderCode;

		static inline std::unordered_map<std::wstring, Constant> _constantBuffers;

		OpenGL::BufferSet::Factory _objectBufferFactory;
		std::unordered_map<std::wstring, Object::Attribute::Factory> _objectAttributeFactories;

		std::string _getFileSection(const std::string& p_inputCode, const std::string& p_delimiter)
		{
			auto startPos = p_inputCode.find(p_delimiter);
			if (startPos == std::string::npos)
			{
				throw std::runtime_error("Delimiter not found in input code.");
			}

			startPos += p_delimiter.length();

			auto endPos = p_inputCode.find("##", startPos);
			if (endPos == std::string::npos)
			{
				endPos = p_inputCode.length();
			}

			return p_inputCode.substr(startPos, endPos - startPos);
		}

		void _parseUniformLayout(Constant::Factory& p_factory, Constant::Layout& p_layoutToFeed, const std::vector<std::string>& p_words, size_t& p_index)
		{
			std::wstring name = StringUtils::stringToWString(p_words[p_index]);
			size_t cpuOffset = std::stoi(p_words[p_index + 1]);
			size_t cpuSize = std::stoi(p_words[p_index + 2]);
			size_t gpuOffset = std::stoi(p_words[p_index + 3]);
			size_t gpuSize = std::stoi(p_words[p_index + 4]);
			Constant::Layout& newLayout = p_factory.addInnerLayout(p_layoutToFeed, name, { cpuOffset, cpuSize }, { gpuOffset, gpuSize });
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
				p_index++;
		}

		std::unordered_map<std::wstring, Constant::Factory> _parseUniformDescriptors(const std::string& p_constantDescriptors)
		{
			std::unordered_map<std::wstring, Constant::Factory> result;

			std::string section = StringUtils::mergeWhitespace(p_constantDescriptors);

			std::vector<std::string> words = StringUtils::splitString(section, " ");

			if (words.size() > 2)
			{
				for (size_t i = 0; i < words.size(); i++)
				{
					std::string uniformType = words[i];
					std::string uniformName = words[i + 1];

					Constant::Factory newFactory;

					newFactory.setTypeName(words[i]);
					newFactory.mainLayout()._cpu.size = std::stoi(words[i + 2]);
					newFactory.mainLayout()._gpu.size = std::stoi(words[i + 3]);

					i += 5; // Skip type, name, cpu size, gpu size and {
					while (words[i] != "}")
					{
						_parseUniformLayout(newFactory, newFactory.mainLayout(), words, i);
					}

					result[StringUtils::stringToWString(uniformName)] = newFactory;
				}
			}

			return result;
		}

		void _parseConstants(const std::string& p_fileContent)
		{
			std::string constantDescriptors = _getFileSection(p_fileContent, CONSTANTS_DELIMITER);

			auto constantFactories = _parseUniformDescriptors(constantDescriptors);

			for (auto& [key, factory] : constantFactories)
			{
				std::wstring typeName = StringUtils::stringToWString(factory.typeName());
				if (_constantBuffers.contains(typeName) == false)
				{
					factory.setBindingPoint(static_cast<int>(_constantBuffers.size()));
					_constantBuffers[typeName] = factory.construct();
				}
				else if (_constantBuffers[typeName].size() != factory.mainLayout()._cpu.size)
				{
					throw std::runtime_error("Constant [" + factory.typeName() + "] already parsed with a different size");
				}
			}
		}

		void _parseAttributes(const std::string& p_fileContent)
		{
			std::string attributeDescriptor = _getFileSection(p_fileContent, ATTRIBUTES_DELIMITER);

			_objectAttributeFactories = _parseUniformDescriptors(attributeDescriptor);

			size_t offset = 0;
			for (auto& [key, factory] : _objectAttributeFactories)
			{
				factory.setBindingPoint(static_cast<int>(_constantBuffers.size() + offset));
				offset++;
			}
		}

		void _applyBindingPoints()
		{
			for (const auto& [key, constantBuffer] : _constantBuffers)
			{
				std::string typeName = constantBuffer.typeName();
				int bindingPoint = constantBuffer.bindingPoint();

				std::regex pattern("layout\\(constant\\) uniform " + typeName + "\\b");
				std::string replacement = "layout(std140, binding = " + std::to_string(bindingPoint) + ") uniform " + typeName;

				_vertexShaderCode = std::regex_replace(_vertexShaderCode, pattern, replacement);
				_fragmentShaderCode = std::regex_replace(_fragmentShaderCode, pattern, replacement);
			}

			for (const auto& [key, attributeFactory] : _objectAttributeFactories)
			{
				std::string typeName = attributeFactory.typeName();
				int bindingPoint = attributeFactory.bindingPoint();

				std::regex pattern("layout\\(attribute\\) uniform " + typeName + "\\b");
				std::string replacement = "layout(std140, binding = " + std::to_string(bindingPoint) + ") uniform " + typeName;

				_vertexShaderCode = std::regex_replace(_vertexShaderCode, pattern, replacement);
				_fragmentShaderCode = std::regex_replace(_fragmentShaderCode, pattern, replacement);
			}
		}

		void _parseLayout(const std::string& p_fileContent)
		{
			std::string layoutDescriptors = _getFileSection(p_fileContent, LAYOUTS_DELIMITER);
			std::vector<std::string> words = StringUtils::splitString(layoutDescriptors, " ");

			for (size_t i = 0; i < words.size(); i += 3)
			{
				GLuint index = static_cast<GLuint>(std::stoi(words[i]));
				size_t size = static_cast<size_t>(std::stoi(words[i + 1]));

				OpenGL::LayoutBufferObject::Attribute::Type type = static_cast<OpenGL::LayoutBufferObject::Attribute::Type>(std::stoi(words[i + 2]));

				_objectBufferFactory.insert(index, size, type);
			}
		}

		void _render(Object& p_object)
		{

		}

	public:
		Pipeline(const std::filesystem::path& p_shaderFile)
		{
			std::string fileContent = FileUtils::readFileAsString(p_shaderFile);

			_vertexShaderCode = _getFileSection(fileContent, VERTEX_DELIMITER);
			_fragmentShaderCode = _getFileSection(fileContent, FRAGMENT_DELIMITER);

			_parseLayout(fileContent);
			_parseConstants(fileContent);
			_parseAttributes(fileContent);

			_applyBindingPoints();

			std::cout << _vertexShaderCode << std::endl;
			std::cout << _fragmentShaderCode << std::endl;
		}

		Object createObject()
		{
			Object result;

			result._owner = this;
			result._bufferSet = _objectBufferFactory.construct();
			for (const auto& [key, factory] : _objectAttributeFactories)
			{
				result._attributes[key] = factory.construct();
			}

			return (result);
		}
	};
}

int main()
{
	try
	{
		spk::Pipeline pipeline("shader/shader.lum");
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}

	return (0);
}