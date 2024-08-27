#include "sparkle.hpp"

#include <GL/glew.h>
#include <string>
#include <stdexcept>
#include <vector>

#include "structure/container/spk_JSON_object.hpp"
#include "structure/graphics/opengl/spk_buffer_set.hpp"
#include "structure/graphics/opengl/spk_uniform_buffer_object.hpp"

namespace spk::OpenGL
{
    class Pipeline
    {
    public:
        using Constant = UniformBufferObject;

        class Object
        {
            friend class Pipeline;

        public:
            using Attribute = UniformBufferObject;

        private:
            Pipeline* _owner;
            BufferSet _bufferSet;
            std::unordered_map<std::wstring, Attribute> _attributes;

        public:
            Object() :
                _owner(nullptr)
            {

            }

            Object(Pipeline* owner) :
                _owner(owner)
            {
            }

            void render()
            {
                _owner->_activate();

                _bufferSet.activate();

                for (auto& [name, attribute] : _attributes)
                {
                    attribute.activate();
                }

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

            Attribute& attribute(const std::wstring& p_name)
            {
                auto it = _attributes.find(p_name);
                if (it == _attributes.end())
                {
                    throw std::out_of_range("No attribute found with the name: " + spk::StringUtils::wstringToString(p_name));
                }
                return it->second;
            }
        };

    private:
        friend class Object;
        std::string _vertexShaderCode;
        std::string _fragmentShaderCode;
        GLuint _programID;

        LayoutBufferObject::Factory _layoutFactory;
        std::unordered_map<std::wstring, Object::Attribute::Factory> _objectAttributeLayouts;

        static inline std::unordered_map<std::wstring, Constant> _constants;

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

            for (auto& [name, constant] : _constants)
            {
                constant.activate();
            }

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
        Pipeline(const spk::JSON::File& p_jsonFile)
        {
            _vertexShaderCode = spk::StringUtils::wstringToString(p_jsonFile[L"VertexShaderCode"].as<std::wstring>());
            _fragmentShaderCode = spk::StringUtils::wstringToString(p_jsonFile[L"FragmentShaderCode"].as<std::wstring>());
            
            _layoutFactory.parse(p_jsonFile[L"Layout"]);

            if (p_jsonFile.contains(L"Constants") == true)
            {
                const auto& constantsArray = p_jsonFile[L"Constants"].asArray();

                for (const auto& constantJson : constantsArray)
                {
                    std::wstring name = (*constantJson)[L"Name"].as<std::wstring>();

                    Constant newConstant = Constant::Factory(*constantJson).construct();

                    if (_constants.contains(name) == true)
                    {
                        if (newConstant.size() != _constants[name].size())
                        {
                            throw std::runtime_error("Constant [" + spk::StringUtils::wstringToString(name) + "] already exist with a different size.");
                        }
                    }
                    else
                    {
                        _constants[name] = std::move(newConstant);
                    }
                }
            }

            if (p_jsonFile.contains(L"Attributes") == true)
            {
                const auto& attributesArray = p_jsonFile[L"Attributes"].asArray();

                for (const auto& attributeJson : attributesArray)
                {
                    std::wstring name = (*attributeJson)[L"Name"].as<std::wstring>();

                    _objectAttributeLayouts[name] = Object::Attribute::Factory(*attributeJson);
                }
            }
        }

        Object createObject()
        {
            Object newObject(this);

            _layoutFactory.apply(&newObject.layout());

            for (auto& [name, attributeLayout] : _objectAttributeLayouts)
            {
                newObject._attributes[name] = attributeLayout.construct();
            }
            return newObject;
        }

        static Constant& constant(const std::wstring& p_name)
        {
            auto it = _constants.find(p_name);
            if (it == _constants.end())
            {
                throw std::out_of_range("No constant found with the name: " + spk::StringUtils::wstringToString(p_name));
            }
            return it->second;
        }
    };
}

class TestWidget : public spk::Widget
{
private:
    spk::OpenGL::Pipeline _pipeline;
    spk::OpenGL::Pipeline::Object _object;
    spk::OpenGL::Pipeline::Constant& _colorAttributes;

	void _onGeometryChange()
	{
        _colorAttributes[L"color"][L"r"] = 1.0f;
        _colorAttributes[L"color"][L"g"] = 0.0f;
        _colorAttributes[L"color"][L"b"] = 1.0f;
        _colorAttributes[L"color"][L"a"] = 1.0f;
        _colorAttributes.validate();

        struct Vertex
        {
            spk::Vector2 position;
        };

        std::vector<Vertex> vertices = {
            {
                spk::Vector2(0, 1)
            },
            {
                spk::Vector2(-1, -1)
            },
            {
                spk::Vector2(1, -1)
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
        _pipeline(spk::JSON::File(L"shader/shader.json")),
        _object(_pipeline.createObject()),
        _colorAttributes(_object.attribute(L"colorBuffer"))
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

