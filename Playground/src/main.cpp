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

namespace spk
{
    class Pipeline
    {
    public:
        using Constant = spk::OpenGL::UniformBufferObject;

    private:
        static inline const std::string LAYOUTS_DELIMITER = "## LAYOUTS DEFINITION ##";
        static inline const std::string CONSTANTS_DELIMITER = "## CONSTANTS DEFINITION ##";
        static inline const std::string ATTRIBUTES_DELIMITER = "## ATTRIBUTES DEFINITION ##";
        static inline const std::string VERTEX_DELIMITER = "## VERTEX SHADER CODE ##";
        static inline const std::string FRAGMENT_DELIMITER = "## FRAGMENT SHADER CODE ##";

        std::string _vertexShaderCode;
        std::string _fragmentShaderCode;

        static inline std::unordered_map<std::wstring, Constant> _constantBuffers;

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

		void parseLayout(Constant::Factory& p_factory, Constant::Layout& p_layoutToFeed, const std::vector<std::string>& p_words, size_t& p_index)
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
                    parseLayout(p_factory, newLayout, p_words, p_index);
                }
                p_index++;
            }
            else if (p_words[p_index] == "{}")
                p_index++;
		}

        std::unordered_map<std::wstring, Constant::Factory> _parseUniformDescriptors(const std::string& p_constantDescriptors)
        {
            std::unordered_map<std::wstring, Constant::Factory> result;
            
            std::vector<std::string> words = StringUtils::splitString(StringUtils::mergeWhitespace(p_constantDescriptors), " ");

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
                    parseLayout(newFactory, newFactory.mainLayout(), words, i);
                }

				result[StringUtils::stringToWString(uniformName)] = newFactory;
            }

            return result;
        }

        void print(const std::wstring p_name, const OpenGL::UniformBufferObject::Layout& p_layout, size_t tabulation)
        {
            spk::cout << std::wstring(tabulation, L' ') << p_name << " - " << p_layout._cpu.offset << " / " << p_layout._cpu.size << std::endl;
            for (const auto& [key, constant] : p_layout._innerLayouts)
            {
                print(key, constant, tabulation + 4);
            }
        }

    public:
        Pipeline(const std::filesystem::path& p_shaderFile)
        {
            std::string fileContent = FileUtils::readFileAsString(p_shaderFile);

            std::string constantDescriptors = _getFileSection(fileContent, CONSTANTS_DELIMITER);

            auto constantFactories = _parseUniformDescriptors(constantDescriptors);
            
            for (auto& [key, factory] : constantFactories)
            {
                std::wstring typeName = StringUtils::stringToWString(factory.typeName());
                if (_constantBuffers.contains(typeName) == false)
                {
                    factory.setBindingPoint(_constantBuffers.size());
                    _constantBuffers[typeName] = factory.construct();
                }
                else if (_constantBuffers[typeName].size() != factory.mainLayout()._cpu.size)
                {
                    throw std::runtime_error("Constant [" + factory.typeName() + "] already parsed with a different size");
                }
                print(key, factory.mainLayout(), 0);
            }

            _vertexShaderCode = _getFileSection(fileContent, VERTEX_DELIMITER);
            _fragmentShaderCode = _getFileSection(fileContent, FRAGMENT_DELIMITER);

            // std::cout << _vertexShaderCode << std::endl;
            // std::cout << _fragmentShaderCode << std::endl;
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
