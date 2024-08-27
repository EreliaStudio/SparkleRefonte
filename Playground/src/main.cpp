#include "sparkle.hpp"

#include <GL/glew.h>
#include <string>
#include <stdexcept>
#include <vector>

#include "structure/container/spk_JSON_object.hpp"
#include "structure/graphics/opengl/spk_buffer_set.hpp"

namespace spk::OpenGL
{
    class Pipeline
    {
    public:
        class Object
        {
        private:
            Pipeline* _owner;
            BufferSet _bufferSet;

        public:
            Object() :
                _owner(nullptr)
            {

            }

            Object(Pipeline* owner)
                : _owner(owner)
            {
            }

            void render()
            {
                _owner->_activate();

                _bufferSet.activate();

                _owner->_draw(_bufferSet.indexes().nbTriangles());

                _bufferSet.deactivate();

                _owner->_deactivate();
            }

            LayoutBufferObject& layout()
            {
                return (_bufferSet.layout());
            }

            IndexBufferObject& indexes()
            {
                return (_bufferSet.indexes());
            }
        };

    private:
        friend class Object;
        std::string _vertexShaderCode;
        std::string _fragmentShaderCode;
        GLuint _programID;
        LayoutBufferObject::Factory _layoutFactory;

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

            DEBUG_LINE();
            glDrawElements(GL_TRIANGLES, p_nbTriangles * 3, GL_UNSIGNED_INT, nullptr);
        }

        void _activate()
        {
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
        Pipeline(const spk::JSON::File& p_jsonFile)
        {
            _vertexShaderCode = spk::StringUtils::wstringToString(p_jsonFile[L"VertexShaderCode"].as<std::wstring>());
            _fragmentShaderCode = spk::StringUtils::wstringToString(p_jsonFile[L"FragmentShaderCode"].as<std::wstring>());
            
            _layoutFactory.parse(p_jsonFile[L"Layout"]);
        }

        Object createObject()
        {
            if (_programID == 0)
                _load();

            Object newObject(this);

            _layoutFactory.apply(&newObject.layout());

            return newObject;
        }
    };
}

class TestWidget : public spk::Widget
{
private:
    spk::OpenGL::Pipeline _pipeline;
    spk::OpenGL::Pipeline::Object _object;

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
        _pipeline(spk::JSON::File(L"shader/shader.json"))
	{
		
	}
};

int main()
{
	spk::GraphicalApplication app;
	
	spk::SafePointer<spk::Window> window = app.createWindow(L"Playground", spk::Geometry2DInt({100, 100}, {800, 800}));
    
	TestWidget testWidget = TestWidget(window->widget());
	testWidget.setGeometry({0, 0, 800, 800});
	testWidget.activate();

	return app.run();
}

