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
    private:
        static inline const std::string LAYOUTS_DELIMITER = "## LAYOUTS DEFINITION ##";
        static inline const std::string CONSTANTS_DELIMITER = "## CONSTANTS DEFINITION ##";
        static inline const std::string ATTRIBUTES_DELIMITER = "## ATTRIBUTES DEFINITION ##";
        static inline const std::string VERTEX_DELIMITER = "## VERTEX SHADER CODE ##";
        static inline const std::string FRAGMENT_DELIMITER = "## FRAGMENT SHADER CODE ##";

        std::string _vertexShaderCode;
        std::string _fragmentShaderCode;

        static std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject> _uniformBuffers;

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

		void parseLayout(spk::OpenGL::UniformBufferObject::Layout& p_layoutToFeed, const std::vector<std::wstring>& p_word, size_t& p_index)
		{

		}

        spk::OpenGL::UniformBufferObject::Factory createFactory(const std::vector<std::string>& p_words, size_t& p_index)
        {
			spk::OpenGL::UniformBufferObject::Factory result;

			result.setTypeName(p_words[p_index]);


			return (result);
        }

        std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject::Factory> _parseConstants(const std::string& p_constantDescriptors)
        {
            std::unordered_map<std::wstring, spk::OpenGL::UniformBufferObject::Factory> constants;
            
            std::vector<std::string> words = StringUtils::splitString(StringUtils::mergeWhitespace(p_constantDescriptors), " ");
            size_t index = 0;


            for (size_t i = 0; i < words.size(); i++)
            {
				std::string uniformType = words[i];
				std::string uniformName = words[i + 1];

				constants[StringUtils::stringToWString(uniformType)] = createFactory(words, index);
            }

            return constants;
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

            std::cout << "Constant file : " << constantDescriptors << std::endl;

            auto parsedConstants = _parseConstants(StringUtils::stringToWString(constantDescriptors));
            
            for (const auto& [key, constant] : parsedConstants)
            {
                print(key, constant.mainLayout(), 0);
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
