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

namespace spk
{
    class Pipeline
    {
    private:
        static inline const std::string VERTEX_DELIMITER = "## VERTEX SHADER CODE ##";
        static inline const std::string FRAGMENT_DELIMITER = "## FRAGMENT SHADER CODE ##";
        std::string _vertexShaderCode;
        std::string _fragmentShaderCode;

        void _readShaders(std::string p_fileContent)
        {
            auto vertexStart = p_fileContent.find(VERTEX_DELIMITER);
            auto fragmentStart = p_fileContent.find(FRAGMENT_DELIMITER);

            if (vertexStart == std::string::npos || fragmentStart == std::string::npos)
            {
                throw std::runtime_error("Shader file is not formatted correctly.");
            }

            vertexStart += VERTEX_DELIMITER.length();
            _vertexShaderCode = StringUtils::trimWhitespace(p_fileContent.substr(vertexStart, fragmentStart - vertexStart));

            fragmentStart += FRAGMENT_DELIMITER.length();
            _fragmentShaderCode = StringUtils::trimWhitespace(p_fileContent.substr(fragmentStart));
        }

    public:
        Pipeline(const std::filesystem::path& p_shaderFile)
        {
            _readShaders(FileUtils::readFileAsString(p_shaderFile));


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
