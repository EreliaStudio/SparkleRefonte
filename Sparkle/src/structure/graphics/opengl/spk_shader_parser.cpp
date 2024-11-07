#include "structure/graphics/opengl/spk_shader_parser.hpp"

#include "utils/spk_string_utils.hpp"

namespace spk
{
	const std::string ShaderParser::LAYOUTS_DELIMITER = "## LAYOUTS DEFINITION ##";
	const std::string ShaderParser::CONSTANTS_DELIMITER = "## CONSTANTS DEFINITION ##";
	const std::string ShaderParser::ATTRIBUTES_DELIMITER = "## ATTRIBUTES DEFINITION ##";
	const std::string ShaderParser::TEXTURES_DELIMITER = "## TEXTURES DEFINITION ##";
	const std::string ShaderParser::VERTEX_DELIMITER = "## VERTEX SHADER CODE ##";
	const std::string ShaderParser::FRAGMENT_DELIMITER = "## FRAGMENT SHADER CODE ##";

	ShaderParser::ConstantInformation::ConstantInformation(const spk::OpenGL::UniformBufferObject::Factory& factory, int bindingPoint, size_t size)
		: factory(factory), bindingPoint(bindingPoint), size(size) {}

	std::string ShaderParser::_getFileSection(const std::string& p_inputCode, const std::string& p_delimiter)
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

	std::string ShaderParser::_cleanAndCondenseWhitespace(const std::string& input)
	{
		std::string result = input;

		std::replace(result.begin(), result.end(), '\t', ' ');
		std::replace(result.begin(), result.end(), '\n', ' ');

		std::regex multipleSpaces("\\s+");
		result = std::regex_replace(result, multipleSpaces, " ");

		result = StringUtils::trimWhitespace(result);

		return result;
	}

	void ShaderParser::_parseUniformLayout(spk::OpenGL::UniformBufferObject::Factory& p_factory, spk::OpenGL::UniformBufferObject::Layout& p_layoutToFeed, const std::vector<std::string>& p_words, size_t& p_index)
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

	std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject::Factory> ShaderParser::_parseUniformDescriptors(const std::string& p_constantDescriptors)
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

	void ShaderParser::_assignBindingPointsToConstants()
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

	void ShaderParser::_assignBindingPointsToAttributes()
	{
		size_t offset = 0;
		for (auto& [key, factory] : _attributeFactories)
		{
			factory.setBindingPoint(static_cast<int>(_constantInfoMap.size() + offset));
			offset++;
		}
	}

	void ShaderParser::_applyBindingPoints(const std::string& type, const std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject::Factory>& factories)
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

	spk::OpenGL::BufferSet::Factory ShaderParser::_parseLayoutDescriptors(const std::string& p_layoutDescriptors)
	{
		std::unordered_map<std::string, std::tuple<size_t, OpenGL::LayoutBufferObject::Attribute::Type> > _typeToSizeMap = {
			{"bool", {1, OpenGL::LayoutBufferObject::Attribute::Type::Byte}},
			{"int", {1, OpenGL::LayoutBufferObject::Attribute::Type::Int}},
			{"uint", {1, OpenGL::LayoutBufferObject::Attribute::Type::UInt}},
			{"float", {1, OpenGL::LayoutBufferObject::Attribute::Type::Float}},

			{"Vector2", {2, OpenGL::LayoutBufferObject::Attribute::Type::Float}},
			{"Vector2Int", {2, OpenGL::LayoutBufferObject::Attribute::Type::Int}},
			{"Vector2UInt", {2, OpenGL::LayoutBufferObject::Attribute::Type::UInt}},

			{"Vector3", {3, OpenGL::LayoutBufferObject::Attribute::Type::Float}},
			{"Vector3Int", {3, OpenGL::LayoutBufferObject::Attribute::Type::Int}},
			{"Vector3UInt", {3, OpenGL::LayoutBufferObject::Attribute::Type::UInt}},

			{"Vector4", {4, OpenGL::LayoutBufferObject::Attribute::Type::Float}},
			{"Vector4Int", {4, OpenGL::LayoutBufferObject::Attribute::Type::Int}},
			{"Vector4UInt", {4, OpenGL::LayoutBufferObject::Attribute::Type::UInt}},
		};
		spk::OpenGL::BufferSet::Factory result;

		std::string section = StringUtils::mergeWhitespace(p_layoutDescriptors);

		std::vector<std::string> words = StringUtils::splitString(section, " ");

		for (size_t i = 0; i < words.size(); i += 4)
		{
			GLuint index = static_cast<GLuint>(std::stoi(words[i]));

			std::string locationString = words[i + 0];
			spk::OpenGL::BufferSet::Direction direction = (words[i + 1] == "in" ? spk::OpenGL::BufferSet::Direction::In : spk::OpenGL::BufferSet::Direction::Out);
			std::string layoutTypeString = words[i + 2];
			std::string layoutNameString = words[i + 3];

			result.insert(
				direction,
				std::stol(locationString),
				std::get<0>(_typeToSizeMap[layoutTypeString]),
				std::get<1>(_typeToSizeMap[layoutTypeString])
			);
		}

		return (result);
	}

	std::unordered_map<std::wstring, spk::OpenGL::SamplerObject::Factory> ShaderParser::_parseSamplerDescriptors(const std::string& p_textureDescriptors)
	{
		std::unordered_map<std::wstring, spk::OpenGL::SamplerObject::Factory> result;

		std::string section = StringUtils::mergeWhitespace(p_textureDescriptors);

		std::vector<std::string> lines = StringUtils::splitString(section, " ");

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

		return result;
	}

	ShaderParser::ShaderParser(const std::string& p_fileContent)
	{
		_vertexShaderCode = _getFileSection(p_fileContent, ShaderParser::VERTEX_DELIMITER);
		_fragmentShaderCode = _getFileSection(p_fileContent, ShaderParser::FRAGMENT_DELIMITER);

		std::string layoutDescriptors = _getFileSection(p_fileContent, ShaderParser::LAYOUTS_DELIMITER);
		_objectBufferFactory = _parseLayoutDescriptors(layoutDescriptors);

		std::string textureDescriptors = _getFileSection(p_fileContent, ShaderParser::TEXTURES_DELIMITER);
		_samplerFactories = _parseSamplerDescriptors(textureDescriptors);

		std::string constantDescriptors = _getFileSection(p_fileContent, ShaderParser::CONSTANTS_DELIMITER);
		_currentConstantFactories = _parseUniformDescriptors(constantDescriptors);
		_assignBindingPointsToConstants();

		std::string attributeDescriptor = _getFileSection(p_fileContent, ShaderParser::ATTRIBUTES_DELIMITER);
		_attributeFactories = _parseUniformDescriptors(attributeDescriptor);
		_assignBindingPointsToAttributes();

		_applyBindingPoints("constant", _currentConstantFactories);
		_applyBindingPoints("attribute", _attributeFactories);
	}

	const std::unordered_map<std::wstring, ShaderParser::ConstantInformation>& ShaderParser::getConstantInfoMap()
	{
		return _constantInfoMap;
	}

	const std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject::Factory>& ShaderParser::getCurrentConstantFactories() const
	{
		return _currentConstantFactories;
	}

	const std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject::Factory>& ShaderParser::getAttributeFactories() const
	{
		return _attributeFactories;
	}

	const spk::OpenGL::BufferSet::Factory& ShaderParser::getLayoutFactory() const
	{
		return _objectBufferFactory;
	}

	const std::unordered_map<std::wstring, spk::OpenGL::SamplerObject::Factory>& ShaderParser::getSamplerFactories() const
	{
		return _samplerFactories;
	}

	const std::string& ShaderParser::getVertexShaderCode() const
	{
		return _vertexShaderCode;
	}

	const std::string& ShaderParser::getFragmentShaderCode() const
	{
		return _fragmentShaderCode;
	}
}
