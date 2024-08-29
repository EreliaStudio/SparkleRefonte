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
    class Parser
    {
    private:
        std::string _vertexShaderCode;
        std::string _fragmentShaderCode;

    public:
        static inline const std::string LAYOUTS_DELIMITER = "## LAYOUTS DEFINITION ##";
        static inline const std::string CONSTANTS_DELIMITER = "## CONSTANTS DEFINITION ##";
        static inline const std::string ATTRIBUTES_DELIMITER = "## ATTRIBUTES DEFINITION ##";
        static inline const std::string VERTEX_DELIMITER = "## VERTEX SHADER CODE ##";
        static inline const std::string FRAGMENT_DELIMITER = "## FRAGMENT SHADER CODE ##";

        std::string getFileSection(const std::string& p_inputCode, const std::string& p_delimiter)
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

        void parseUniformLayout(spk::OpenGL::UniformBufferObject::Factory& p_factory, spk::OpenGL::UniformBufferObject::Layout& p_layoutToFeed, const std::vector<std::string>& p_words, size_t& p_index)
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
                    parseUniformLayout(p_factory, newLayout, p_words, p_index);
                }
                p_index++;
            }
            else if (p_words[p_index] == "{}")
                p_index++;
        }

        std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject::Factory> parseUniformDescriptors(const std::string& p_constantDescriptors)
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

                    i += 5; // Skip type, name, cpu size, gpu size and {
                    while (words[i] != "}")
                    {
                        parseUniformLayout(newFactory, newFactory.mainLayout(), words, i);
                    }

                    result[StringUtils::stringToWString(uniformName)] = newFactory;
                }
            }

            return result;
        }

        std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject> parseConstants(const std::string& p_fileContent)
        {
            std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject> constantBuffers;

            std::string constantDescriptors = getFileSection(p_fileContent, CONSTANTS_DELIMITER);

            auto constantFactories = parseUniformDescriptors(constantDescriptors);

            for (auto& [key, factory] : constantFactories)
            {
                std::wstring typeName = StringUtils::stringToWString(factory.typeName());
                if (constantBuffers.contains(typeName) == false)
                {
                    factory.setBindingPoint(static_cast<int>(constantBuffers.size()));
                    constantBuffers[typeName] = factory.construct();
                }
                else if (constantBuffers[typeName].size() != factory.mainLayout()._cpu.size)
                {
                    throw std::runtime_error("Constant [" + factory.typeName() + "] already parsed with a different size");
                }
            }

            return constantBuffers;
        }

        std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject::Factory> parseAttributes(const std::string& p_fileContent, const std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject>& constantBuffers)
        {
            std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject::Factory> objectAttributeFactories;

            std::string attributeDescriptor = getFileSection(p_fileContent, ATTRIBUTES_DELIMITER);

            objectAttributeFactories = parseUniformDescriptors(attributeDescriptor);

            size_t offset = 0;
            for (auto& [key, factory] : objectAttributeFactories)
            {
                factory.setBindingPoint(static_cast<int>(constantBuffers.size() + offset));
                offset++;
            }

            return objectAttributeFactories;
        }

        spk::OpenGL::BufferSet::Factory parseLayout(const std::string& p_fileContent)
        {
            spk::OpenGL::BufferSet::Factory objectBufferFactory;
            std::string layoutDescriptors = getFileSection(p_fileContent, LAYOUTS_DELIMITER);
            std::vector<std::string> words = StringUtils::splitString(layoutDescriptors, " ");

            for (size_t i = 0; i < words.size(); i += 3)
            {
                GLuint index = static_cast<GLuint>(std::stoi(words[i]));
                size_t size = static_cast<size_t>(std::stoi(words[i + 1]));

                spk::OpenGL::LayoutBufferObject::Attribute::Type type = static_cast<spk::OpenGL::LayoutBufferObject::Attribute::Type>(std::stoi(words[i + 2]));

                objectBufferFactory.insert(index, size, type);
            }

            return objectBufferFactory;
        }

        void applyBindingPoints(
            const std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject>& constantBuffers,
            const std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject::Factory>& objectAttributeFactories)
        {
            for (const auto& [key, constantBuffer] : constantBuffers)
            {
                std::string typeName = constantBuffer.typeName();
                int bindingPoint = constantBuffer.bindingPoint();

                std::regex pattern("layout\\(constant\\) uniform " + typeName + "\\b");
                std::string replacement = "layout(std140, binding = " + std::to_string(bindingPoint) + ") uniform " + typeName;

                _vertexShaderCode = std::regex_replace(_vertexShaderCode, pattern, replacement);
                _fragmentShaderCode = std::regex_replace(_fragmentShaderCode, pattern, replacement);
            }

            for (const auto& [key, attributeFactory] : objectAttributeFactories)
            {
                std::string typeName = attributeFactory.typeName();
                int bindingPoint = attributeFactory.bindingPoint();

                std::regex pattern("layout\\(attribute\\) uniform " + typeName + "\\b");
                std::string replacement = "layout(std140, binding = " + std::to_string(bindingPoint) + ") uniform " + typeName;

                _vertexShaderCode = std::regex_replace(_vertexShaderCode, pattern, replacement);
                _fragmentShaderCode = std::regex_replace(_fragmentShaderCode, pattern, replacement);
            }
        }

        void parseShaderCode(const std::string& p_fileContent)
        {
            _vertexShaderCode = getFileSection(p_fileContent, VERTEX_DELIMITER);
            _fragmentShaderCode = getFileSection(p_fileContent, FRAGMENT_DELIMITER);
        }

        const std::string& vertexShaderCode() const
        {
            return _vertexShaderCode;
        }

        const std::string& fragmentShaderCode() const
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
            spk::OpenGL::BufferSet _bufferSet;
            std::unordered_map<std::wstring, Attribute::Factory> _attributes;

            void _activate()
            {
                _bufferSet.activate();

                for (auto& [name, attribute] : _attributes)
                {
                    attribute.construct().activate();
                }
            }

            void _deactivate()
            {
                _bufferSet.deactivate();

                for (auto& [name, attribute] : _attributes)
                {
                    attribute.construct().deactivate();
                }
            }

        public:
            Object() = default;

            spk::OpenGL::LayoutBufferObject& layout()
            {
                return (_bufferSet.layout());
            }

            spk::OpenGL::IndexBufferObject& indexes()
            {
                return (_bufferSet.indexes());
            }

            void render()
            {
                if (_owner == nullptr)
                {
                    return;
                }

                _owner->_render(*this, _bufferSet.indexes().nbTriangles());
            }
        };

    private:
        std::string _vertexShaderCode;
        std::string _fragmentShaderCode;

        static inline std::unordered_map<std::wstring, Constant> _constantBuffers;

        spk::OpenGL::BufferSet::Factory _objectBufferFactory;
        std::unordered_map<std::wstring, Object::Attribute::Factory> _objectAttributeFactories;

        void _render(Object& p_object, size_t p_nbTriangle)
        {
            // Rendering logic here
        }

    public:
        Pipeline(const std::filesystem::path& p_shaderFile)
        {
            std::string fileContent = FileUtils::readFileAsString(p_shaderFile);
            Parser parser;

            parser.parseShaderCode(fileContent);
            _vertexShaderCode = parser.vertexShaderCode();
            _fragmentShaderCode = parser.fragmentShaderCode();

            _objectBufferFactory = parser.parseLayout(fileContent);
            _constantBuffers = parser.parseConstants(fileContent);
            _objectAttributeFactories = parser.parseAttributes(fileContent, _constantBuffers);

            parser.applyBindingPoints(_constantBuffers, _objectAttributeFactories);

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
                result._attributes.emplace(key, factory.construct());
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
