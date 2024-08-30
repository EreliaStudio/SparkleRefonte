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
		static inline const std::string VERTEX_DELIMITER = "## VERTEX SHADER CODE ##";
		static inline const std::string FRAGMENT_DELIMITER = "## FRAGMENT SHADER CODE ##";

		std::string _vertexShaderCode;
		std::string _fragmentShaderCode;

		static inline std::unordered_map<std::wstring, ConstantInformation> _constantInfoMap;
		std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject::Factory> _currentConstantFactories;
		std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject::Factory> _attributeFactories;
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

	public:
		ShaderParser(const std::string& p_fileContent)
		{
			_vertexShaderCode = _getFileSection(p_fileContent, ShaderParser::VERTEX_DELIMITER);
			_fragmentShaderCode = _getFileSection(p_fileContent, ShaderParser::FRAGMENT_DELIMITER);

			std::string layoutDescriptors = _getFileSection(p_fileContent, ShaderParser::LAYOUTS_DELIMITER);
			_objectBufferFactory = _parseLayoutDescriptors(layoutDescriptors);

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

		const std::string& getVertexShaderCode() const
		{
			return _vertexShaderCode;
		}

		const std::string& getFragmentShaderCode() const
		{
			return _fragmentShaderCode;
		}
	};

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
		ShaderParser _parser;

		GLuint _programID;

		static inline std::unordered_map<std::wstring, Constant> _constants;
		std::unordered_map<std::wstring, Constant*> _activeConstants;

		GLuint _compileShader(const std::string& shaderCode, GLenum shaderType)
		{
			GLuint shader = glCreateShader(shaderType);
			const char* source = shaderCode.c_str();
			glShaderSource(shader, 1, &source, nullptr);
			glCompileShader(shader);

			GLint success;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				GLchar infoLog[512];
				glGetShaderInfoLog(shader, 512, nullptr, infoLog);
				std::string shaderTypeStr = (shaderType == GL_VERTEX_SHADER) ? "Vertex Shader" : "Fragment Shader";
				throw std::runtime_error(shaderTypeStr + " compilation failed: " + std::string(infoLog));
			}

			return shader;
		}

		GLuint _linkProgram(GLuint vertexShader, GLuint fragmentShader)
		{
			GLuint program = glCreateProgram();
			glAttachShader(program, vertexShader);
			glAttachShader(program, fragmentShader);
			glLinkProgram(program);

			GLint success;
			glGetProgramiv(program, GL_LINK_STATUS, &success);
			if (!success)
			{
				GLchar infoLog[512];
				glGetProgramInfoLog(program, 512, nullptr, infoLog);
				throw std::runtime_error("Shader Program linking failed: " + std::string(infoLog));
			}

			// Clean up shaders as they're now linked into the program and no longer needed
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			return program;
		}

		void _load()
		{
			std::cout << "Vertex code : " << std::endl << "[" << _parser.getVertexShaderCode() << "]" << std::endl;
			std::cout << "Fragment code : " << std::endl << "[" << _parser.getFragmentShaderCode() << "]" << std::endl;

			GLuint vertexShader = _compileShader(_parser.getVertexShaderCode(), GL_VERTEX_SHADER);
			GLuint fragmentShader = _compileShader(_parser.getFragmentShaderCode(), GL_FRAGMENT_SHADER);
			_programID = _linkProgram(vertexShader, fragmentShader);
		}

		void _activate()
		{
			glUseProgram(_programID);
		}

		void _deactivate()
		{
			glUseProgram(0);
		}

		void _render(Object& p_object)
		{
			if (_programID == 0)
				_load();

			_activate();

			p_object._activate();

			for (auto& [name, constant] : _activeConstants)
			{
				constant->activate();
			}

			glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(p_object.indexes().nbIndexes()), GL_UNSIGNED_INT, nullptr);

			for (auto& [name, constant] : _activeConstants)
			{
				constant->deactivate();
			}

			p_object._deactivate();

			_deactivate();
		}

	public:
		Pipeline(const std::filesystem::path& p_shaderFile) :
			_parser(FileUtils::readFileAsString(p_shaderFile))
		{
			for (const auto& [key, factory] : _parser.getCurrentConstantFactories())
			{
				if (_constants.contains(key) == false)
				{
					_constants[key] = factory.construct();
				}

				_activeConstants[key] = &(_constants[key]);
			}
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

			return (result);
		}
	};
}

class TestWidget : public spk::Widget
{
private:
	spk::Pipeline _pipeline;
	spk::Pipeline::Object _object;

	void _onGeometryChange()
	{
		_object = _pipeline.createObject();

		struct Vertex
		{
			spk::Vector2 position;
			spk::Color color;
		};

		std::vector<Vertex> vertices = {
			{
				spk::Vector2(0, 1),
				spk::Color::red
			},
			{
				spk::Vector2(-1, -1),
				spk::Color::blue
			},
			{
				spk::Vector2(1, -1),
				spk::Color::green
			}
		};
		std::vector<unsigned int> indexes = {
			0, 1, 2
		};

		_object.layout() << vertices;
		_object.indexes() << indexes;

		_object.layout().validate();
		_object.indexes().validate();
	}

	void _onPaintEvent(const spk::PaintEvent& p_event)
	{
		_object.render();
	}

public:
	TestWidget(spk::SafePointer<Widget> p_parent) :
		spk::Widget(L"TestWidget", p_parent),
		_pipeline(L"shader/shader.lum")
	{

	}
};

int main()
{
	spk::GraphicalApplication app;
	
	spk::SafePointer<spk::Window> window = app.createWindow(L"Playground", spk::Geometry2DInt({ 100, 100 }, { 800, 800 }));

	TestWidget testWidget = TestWidget(window->widget());
	testWidget.setGeometry({ 0, 0, 800, 800 });
	testWidget.activate();

	return app.run();

}