#include "sparkle.hpp"

#include <GL/glew.h>
#include <string>
#include <stdexcept>
#include <vector>

namespace spk::OpenGL
{
    class Pipeline
    {
    private:
        std::string _vertexShaderCode;
        std::string _fragmentShaderCode;
        GLuint _programID;

        GLuint _compileShader(const std::string& source, GLenum shaderType)
        {
            GLuint shaderID = glCreateShader(shaderType);
            const char* sourceCStr = source.c_str();
            glShaderSource(shaderID, 1, &sourceCStr, nullptr);
            glCompileShader(shaderID);

            GLint compileStatus = GL_FALSE;
            glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileStatus);
            if (compileStatus == GL_FALSE)
            {
                GLint infoLogLength;
                glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
                std::vector<char> shaderErrorMessage(infoLogLength);
                glGetShaderInfoLog(shaderID, infoLogLength, nullptr, shaderErrorMessage.data());
                std::string errorMessage(shaderErrorMessage.begin(), shaderErrorMessage.end());
                throw std::runtime_error("Shader compilation error: " + errorMessage);
            }

            return shaderID;
        }

        void _linkProgram(GLuint vertexShaderID, GLuint fragmentShaderID)
        {
            _programID = glCreateProgram();
            glAttachShader(_programID, vertexShaderID);
            glAttachShader(_programID, fragmentShaderID);
            glLinkProgram(_programID);

            GLint linkStatus = GL_FALSE;
            glGetProgramiv(_programID, GL_LINK_STATUS, &linkStatus);
            if (linkStatus == GL_FALSE)
            {
                GLint infoLogLength;
                glGetProgramiv(_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
                std::vector<char> programErrorMessage(infoLogLength);
                glGetProgramInfoLog(_programID, infoLogLength, nullptr, programErrorMessage.data());
                std::string errorMessage(programErrorMessage.begin(), programErrorMessage.end());
                throw std::runtime_error("Program linking error: " + errorMessage);
            }
        }

        void _draw(size_t p_nbTriangles)
        {
            if (p_nbTriangles == 0)
                return;

            glDrawElements(GL_TRIANGLES, p_nbTriangles * 3, GL_UNSIGNED_INT, nullptr);
        }

        void _activate()
        {
            if (_programID == 0)
                _load();

            glUseProgram(_programID);
        }

        void _deactivate()
        {
            glUseProgram(0);
        }

        void _load()
        {
            GLuint vertexShaderID = _compileShader(_vertexShaderCode, GL_VERTEX_SHADER);
            GLuint fragmentShaderID = _compileShader(_fragmentShaderCode, GL_FRAGMENT_SHADER);

            _linkProgram(vertexShaderID, fragmentShaderID);

            glDeleteShader(vertexShaderID);
            glDeleteShader(fragmentShaderID);
        }

    public:
        Pipeline(const std::filesystem::path& p_shaderFile)
        {
            _vertexShaderCode = ;
            _fragmentShaderCode = ;
            
        }
    };
}

class TestWidget : public spk::Widget
{
private:
    spk::OpenGL::Pipeline _pipeline;

	void _onGeometryChange()
	{
	}
	
	void _onPaintEvent(const spk::PaintEvent& p_event)
	{

	}

public:
	TestWidget(spk::SafePointer<Widget> p_parent) :
		spk::Widget(L"TestWidget", p_parent),
        _pipeline(spk::JSON::File(L"shader/shader.json"))
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

