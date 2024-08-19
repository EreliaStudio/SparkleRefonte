#include "sparkle.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

namespace spk
{
class Pipeline
{
public:
    class VertexBufferObject
    {
    public:
        enum class Type : GLenum
        {
            Storage = GL_ARRAY_BUFFER,               // Vertex attributes
            Element = GL_ELEMENT_ARRAY_BUFFER,       // Element indices
            Uniform = GL_UNIFORM_BUFFER,             // Uniform block storage
            Texture = GL_TEXTURE_BUFFER,             // Texture data storage
            TransformFeedback = GL_TRANSFORM_FEEDBACK_BUFFER,  // Transform feedback
            ShaderStorage = GL_SHADER_STORAGE_BUFFER, // Shader storage block storage
            PixelPack = GL_PIXEL_PACK_BUFFER,        // Pixel readback
            PixelUnpack = GL_PIXEL_UNPACK_BUFFER,    // Texture data upload
            DrawIndirect = GL_DRAW_INDIRECT_BUFFER,  // Indirect drawing commands
            AtomicCounter = GL_ATOMIC_COUNTER_BUFFER // Atomic counter storage
        };

        enum class Usage : GLenum
        {
            Static = GL_STATIC_DRAW,     // Data will be set once and used many times
            Dynamic = GL_DYNAMIC_DRAW,   // Data will be changed frequently and used many times
            Stream = GL_STREAM_DRAW,     // Data will be set once and used a few times
            StaticRead = GL_STATIC_READ, // Data will be read back, and will be written by OpenGL once and read many times
            DynamicRead = GL_DYNAMIC_READ, // Data will be read back, and will be written by OpenGL repeatedly and read many times
            StreamRead = GL_STREAM_READ,   // Data will be read back, and will be written by OpenGL once and read a few times
            StaticCopy = GL_STATIC_COPY,   // Data will be copied to another buffer, written by OpenGL once and copied many times
            DynamicCopy = GL_DYNAMIC_COPY, // Data will be copied to another buffer, written by OpenGL repeatedly and copied many times
            StreamCopy = GL_STREAM_COPY    // Data will be copied to another buffer, written by OpenGL once and copied a few times
        };

    private:
        GLuint _id;
        Type _type;
        Usage _usage;
        GLsizeiptr _currentSize;
        spk::DataBuffer _datas;
        bool _needsUpdate;
        mutable std::mutex _mutex;

    public:
        VertexBufferObject(Type p_type, Usage p_usage) :
            _type(p_type),
            _usage(p_usage),
            _currentSize(0),
            _needsUpdate(false)
        {
            glGenBuffers(1, &_id);
        }

        VertexBufferObject(const VertexBufferObject& other) :
            _type(other._type),
            _usage(other._usage),
            _currentSize(other._currentSize),
            _datas(other._datas),
            _needsUpdate(other._needsUpdate)
        {
            glGenBuffers(1, &_id);
            glBindBuffer(static_cast<GLenum>(_type), _id);
            glBufferData(static_cast<GLenum>(_type), _datas.size(), _datas.data(), static_cast<GLenum>(_usage));
        }

        VertexBufferObject(VertexBufferObject&& other) noexcept :
            _id(other._id),
            _type(other._type),
            _usage(other._usage),
            _currentSize(other._currentSize),
            _datas(std::move(other._datas)),
            _needsUpdate(other._needsUpdate)
        {
            other._id = 0;
        }

        ~VertexBufferObject()
        {
            glDeleteBuffers(1, &_id);
        }

        VertexBufferObject& operator=(const VertexBufferObject& other)
        {
            if (this != &other)
            {
                glDeleteBuffers(1, &_id);

                _type = other._type;
                _usage = other._usage;
                _currentSize = other._currentSize;
                _datas = other._datas;
                _needsUpdate = other._needsUpdate;

                glGenBuffers(1, &_id);
                glBindBuffer(static_cast<GLenum>(_type), _id);
                glBufferData(static_cast<GLenum>(_type), _datas.size(), _datas.data(), static_cast<GLenum>(_usage));
            }
            return *this;
        }

        VertexBufferObject& operator=(VertexBufferObject&& other) noexcept
        {
            if (this != &other)
            {
                glDeleteBuffers(1, &_id);

                _id = other._id;
                _type = other._type;
                _usage = other._usage;
                _currentSize = other._currentSize;
                _datas = std::move(other._datas);
                _needsUpdate = other._needsUpdate;

                other._id = 0;
            }
            return *this;
        }

        unsigned int id() const
        {
            return (_id);
        }

        Type type() const
        {
            return (_type);
        }

        size_t size() const
        {
            return (_datas.size());
        }

        Usage usage() const
        {
            return (_usage);
        }

        void editUsage(Usage p_usage)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _usage = p_usage;
            _currentSize = 0;
        }

        void activate() const
        {
            std::lock_guard<std::mutex> lock(_mutex);
            glBindBuffer(static_cast<GLenum>(_type), _id);
        }

        void deactivate() const
        {
            std::lock_guard<std::mutex> lock(_mutex);
            glBindBuffer(static_cast<GLenum>(_type), 0);
        }

        void* map(GLenum access) const
        {
            std::lock_guard<std::mutex> lock(_mutex);
            activate();
            void* ptr = glMapBuffer(static_cast<GLenum>(_type), access);
            deactivate();
            return ptr;
        }

        void unmap() const
        {
            std::lock_guard<std::mutex> lock(_mutex);
            activate();
            glUnmapBuffer(static_cast<GLenum>(_type));
            deactivate();
        }

        void reserve(size_t p_size)
        {
            if (_datas.size() <= p_size)
                _datas.resize(p_size);
        }

        void push(const void* p_data, GLsizeiptr p_size, GLsizeiptr p_offset = 0)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (_datas.size() <= static_cast<size_t>(p_size + p_offset))
                _datas.resize(p_size + p_offset);
            _datas.edit(p_offset, p_data, p_size);
        }

        template <typename TType>
        void push(const TType& p_data, GLsizeiptr p_offset = 0)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            push(&p_data, sizeof(TType), p_offset);
        }

        void append(const void* p_data, GLsizeiptr p_size)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _datas.append(p_data, p_size);
        }

        template <typename TType>
        void append(const TType& p_data)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            append(&p_data, sizeof(TType));
        }

        void validate()
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _needsUpdate = true;
        }

        void bind()
        {
            std::lock_guard<std::mutex> lock(_mutex);
            activate();

            if (_needsUpdate == true && _datas.size() > 0)
            {
                if (_datas.size() <= static_cast<size_t>(_currentSize))
                {
                    glBufferSubData(static_cast<GLenum>(_type), 0, _datas.size(), _datas.data());
                }
                else
                {
                    glBufferData(static_cast<GLenum>(_type), _datas.size(), _datas.data(), static_cast<GLenum>(_usage));
                    _currentSize = _datas.size();
                }

                _needsUpdate = false;
            }

            deactivate();
        }

        void bzero()
        {
            std::lock_guard<std::mutex> lock(_mutex);
            ::memset(_datas.data(), '\0', _datas.size());
        }

        VertexBufferObject duplicate() const
        {
            std::lock_guard<std::mutex> lock(_mutex);
            VertexBufferObject copy(_type, _usage);
            copy.push(_datas.data(), _datas.size());
            return copy;
        }

        friend std::wostream& operator<<(std::wostream& p_os, Pipeline::VertexBufferObject::Type p_type);
        friend std::wostream& operator<<(std::wostream& p_os, Pipeline::VertexBufferObject::Usage p_usage);
    };


    class VertexArrayObject
    {
    private:
        GLuint _id;
        bool _isBound;

    public:
        VertexArrayObject() :
            _id(0),
            _isBound(false)
        {
            glGenVertexArrays(1, &_id);
            activate();
        }

        VertexArrayObject(const VertexArrayObject&) = delete; // Technicly complicated to copy a VAO, as it will need to copy internal state
        VertexArrayObject& operator=(const VertexArrayObject&) = delete;

        VertexArrayObject(VertexArrayObject&& other) noexcept :
            _id(other._id),
            _isBound(other._isBound)
        {
            other._id = 0;
            other._isBound = false;
        }

        ~VertexArrayObject()
        {
            glDeleteVertexArrays(1, &_id);
        }

        VertexArrayObject& operator=(VertexArrayObject&& other) noexcept
        {
            if (this != &other)
            {
                glDeleteVertexArrays(1, &_id);

                _id = other._id;
                _isBound = other._isBound;

                other._id = 0;
                other._isBound = false;
            }
            return *this;
        }

        void activate() const
        {
            glBindVertexArray(_id);
        }

        void deactivate() const 
        {
            glBindVertexArray(0);
        }

        GLuint id() const
        {
            return _id;
        }
    };

    struct Storage
    {
    private:
        VertexArrayObject _vao;
        VertexBufferObject _layoutBuffer;
        VertexBufferObject _elementBuffer;

    public:
        Storage() :
            _vao(),
            _layoutBuffer(VertexBufferObject::Type::Storage, VertexBufferObject::Usage::Static),
            _elementBuffer(VertexBufferObject::Type::Element, VertexBufferObject::Usage::Static)
        {
        }

        void activate()
        {
            _vao.activate();
            _layoutBuffer.bind();
            _elementBuffer.bind();
        }

        void deactivate()
        {
            _layoutBuffer.deactivate();
            _elementBuffer.deactivate();
            _vao.deactivate();
        }

        VertexArrayObject& vao()
        {
            return _vao;
        }

        const VertexArrayObject& vao() const
        {
            return _vao;
        }

        VertexBufferObject& layoutBuffer()
        {
            return _layoutBuffer;
        }

        const VertexBufferObject& layoutBuffer() const
        {
            return _layoutBuffer;
        }

        VertexBufferObject& elementBuffer()
        {
            return _elementBuffer;
        }

        const VertexBufferObject& elementBuffer() const
        {
            return _elementBuffer;
        }

        void bzero()
        {
            _layoutBuffer.bzero();
            _elementBuffer.bzero();
        }
    };

    class Uniform
    {
    public:
		using BindingPoint = long long;

		class Element
        {
        private:
            GLint _gpuOffset;
            GLsizeiptr _size;
            GLsizeiptr _cpuOffset;
            VertexBufferObject* _vbo;

        public:
            Element(GLint p_gpuOffset, GLsizeiptr p_size, GLsizeiptr p_cpuOffset, VertexBufferObject* p_vbo)
                : _gpuOffset(p_gpuOffset), _size(p_size), _cpuOffset(p_cpuOffset), _vbo(p_vbo)
            {}

            template <typename TType>
            Element& operator=(const TType& p_data)
            {
                if (sizeof(TType) != _size)
                {
                    throw std::runtime_error("Size mismatch in Uniform::Element assignment. "
                        "Expected a size of " + std::to_string(_size) + " bytes, "
                        "but received " + std::to_string(sizeof(TType)) + " bytes of data.");
                }
                _vbo->push(&p_data, _size, _gpuOffset);
                return *this;
            }

            void setDataFromBytes(const void* p_basePtr)
            {
                const void* dataPtr = static_cast<const char*>(p_basePtr) + _cpuOffset;
                _vbo->push(dataPtr, _size, _gpuOffset);
            }
        };

    private:
        VertexBufferObject _vbo;
        GLuint _bindingPoint;
        std::unordered_map<std::wstring, Element> _elements;

    public:
        Uniform(const spk::JSON::Object& p_jsonConfig, GLuint p_bindingPoint)
            : _vbo(VertexBufferObject::Type::Uniform, VertexBufferObject::Usage::Static),
            _bindingPoint(p_bindingPoint)
        {
            if (!p_jsonConfig.contains(L"size") || !p_jsonConfig.contains(L"elements"))
            {
                throw std::runtime_error("Invalid JSON configuration for Uniform.");
            }

            GLsizeiptr bufferSize = static_cast<GLsizeiptr>(p_jsonConfig[L"size"].as<long>());
            _vbo.reserve(bufferSize); 

            const auto& members = p_jsonConfig[L"elements"].members();

            for (const auto& [key, value] : members)
            {
                std::wstring elementName = key;
                GLint gpuOffset = static_cast<GLint>(value->operator[](L"gpuOffset").as<long>());
                GLsizeiptr size = static_cast<GLsizeiptr>(value->operator[](L"size").as<long>());
                GLsizeiptr cpuOffset = static_cast<GLsizeiptr>(value->operator[](L"cpuOffset").as<long>());

                _elements.emplace(elementName, Element(gpuOffset, size, cpuOffset, &_vbo));
            }
        }

        Element& operator[](const std::wstring& p_name)
        {
            auto it = _elements.find(p_name);
            if (it == _elements.end())
            {
                throw std::runtime_error("Uniform element name not found: " + std::string(p_name.begin(), p_name.end()));
            }
            return it->second;
        }

        template <typename TType>
        Uniform& operator=(const TType& p_data)
        {
            const void* basePtr = &p_data;
            for (auto& [name, element] : _elements)
            {
                element.setDataFromBytes(basePtr);
            }
            return *this;
        }

        void bind() const
        {
            _vbo.activate();
            glBindBufferBase(GL_UNIFORM_BUFFER, _bindingPoint, _vbo.id());
        }

        void unbind() const
        {
            glBindBufferBase(GL_UNIFORM_BUFFER, _bindingPoint, 0);
            _vbo.deactivate();
        }

        void validate()
        {
            _vbo.validate();
        }
    };

	class Object
	{
	friend class Pipeline;

	private:
		Pipeline* _owner;
		Storage _storage;
		std::unordered_map<std::wstring, Uniform> _attributes;

		Object(Pipeline* p_owner) :
			_owner(p_owner)
		{

		}

	public:
		Storage& storage()
		{
			return (_storage);
		}

		const Storage& storage() const
		{
			return (_storage);
		}

		std::unordered_map<std::wstring, Uniform>& attributes()
		{
			return (_attributes);
		}

		const std::unordered_map<std::wstring, Uniform>& attributes() const
		{
			return (_attributes);
		}

		void render()
		{
			_owner->activate();

			_storage.activate();
			for (const auto& [key, uniform] : _attributes)
			{
				uniform.bind();
			}
	
			_owner->draw(_storage.elementBuffer().size() / sizeof(size_t));

			_owner->deactivate();
		}

		void renderInstanced(size_t p_nbInstance)
		{
			_owner->activate();

			_storage.activate();
			for (const auto& [key, uniform] : _attributes)
			{
				uniform.bind();
			}
			
			_owner->drawInstanced(_storage.elementBuffer().size() / sizeof(size_t), p_nbInstance);

			_owner->deactivate();
		}
	};

private:
	std::wstring _name;

	bool _loaded;
	GLuint _id;

	std::string _vertexCode;
	std::string _fragmentCode;

	std::unordered_map<std::wstring, Uniform> _constants;

	GLuint compileShader(const std::string& p_code, GLenum mode)
	{
		GLuint shader = glCreateShader(mode);

		const char* shaderSource = p_code.c_str();
		glShaderSource(shader, 1, &shaderSource, nullptr);

		glCompileShader(shader);

		GLint success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			GLchar infoLog[512];
			glGetShaderInfoLog(shader, 512, nullptr, infoLog);
			throw std::runtime_error("Shader compilation failed: " + std::string(infoLog));
		}

		return shader;
	}

	GLuint linkShaders(GLuint vertexShader, GLuint fragmentShader)
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

		return program;
	}

	void activate()
	{
		glUseProgram(_id);
	}

	void deactivate()
	{
		glUseProgram(0);
	}

	void draw(size_t p_nbTriangle)
	{
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(p_nbTriangle), GL_UNSIGNED_INT, nullptr);
	}

	void drawInstanced(size_t p_nbTriangle, size_t p_nbInstance)
	{
		glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(p_nbTriangle), GL_UNSIGNED_INT, nullptr, static_cast<GLsizei>(p_nbInstance));
	}

public:
	Pipeline(const spk::JSON::File& p_inputFile) : 
		_loaded(false)
	{
		_vertexCode = spk::StringUtils::wstringToString(p_inputFile[L"VertexShaderCode"].as<std::wstring>());
		_fragmentCode = spk::StringUtils::wstringToString(p_inputFile[L"FragmentShaderCode"].as<std::wstring>());
	}

	void load()
	{
		GLuint vertexShaderID = compileShader(_vertexCode, GL_VERTEX_SHADER);
		GLuint fragmentShaderID = compileShader(_fragmentCode, GL_FRAGMENT_SHADER);

		_id = linkShaders(vertexShaderID, fragmentShaderID);
		
		glDeleteShader(vertexShaderID);
		glDeleteShader(fragmentShaderID);
	}

	const std::wstring& name() const
	{
		return (_name);
	}
};
std::wstring to_wstring(Pipeline::VertexBufferObject::Type type);
std::wstring to_wstring(Pipeline::VertexBufferObject::Usage usage);
}

namespace spk
{
    std::wstring to_wstring(Pipeline::VertexBufferObject::Type type)
    {
        switch (type)
        {
        case Pipeline::VertexBufferObject::Type::Storage:
            return L"Storage";
        case Pipeline::VertexBufferObject::Type::Element:
            return L"Element";
        case Pipeline::VertexBufferObject::Type::Uniform:
            return L"Uniform";
        case Pipeline::VertexBufferObject::Type::Texture:
            return L"Texture";
        case Pipeline::VertexBufferObject::Type::TransformFeedback:
            return L"Transform Feedback";
        case Pipeline::VertexBufferObject::Type::ShaderStorage:
            return L"Shader Storage";
        case Pipeline::VertexBufferObject::Type::PixelPack:
            return L"Pixel Pack";
        case Pipeline::VertexBufferObject::Type::PixelUnpack:
            return L"Pixel Unpack";
        case Pipeline::VertexBufferObject::Type::DrawIndirect:
            return L"Draw Indirect";
        case Pipeline::VertexBufferObject::Type::AtomicCounter:
            return L"Atomic Counter";
        default:
            return L"Unknown buffer type";
        }
    }

    std::wstring to_wstring(Pipeline::VertexBufferObject::Usage usage)
    {
        switch (usage)
        {
        case Pipeline::VertexBufferObject::Usage::Static:
            return L"Static";
        case Pipeline::VertexBufferObject::Usage::Dynamic:
            return L"Dynamic";
        case Pipeline::VertexBufferObject::Usage::Stream:
            return L"Stream";
        case Pipeline::VertexBufferObject::Usage::StaticRead:
            return L"Static Read";
        case Pipeline::VertexBufferObject::Usage::DynamicRead:
            return L"Dynamic Read";
        case Pipeline::VertexBufferObject::Usage::StreamRead:
            return L"Stream Read";
        case Pipeline::VertexBufferObject::Usage::StaticCopy:
            return L"Static Copy";
        case Pipeline::VertexBufferObject::Usage::DynamicCopy:
            return L"Dynamic Copy";
        case Pipeline::VertexBufferObject::Usage::StreamCopy:
            return L"Stream Copy";
        default:
            return L"Unknown buffer usage";
        }
    }
}

std::wostream& operator<<(std::wostream& os, spk::Pipeline::VertexBufferObject::Type type)
{
    os << spk::to_wstring(type);
    return os;
}

std::wostream& operator<<(std::wostream& os, spk::Pipeline::VertexBufferObject::Usage usage)
{
    os << spk::to_wstring(usage);
    return os;
}

class TestWidget : public spk::Widget
{
private:
	spk::Pipeline _pipeline;

	void _onGeometryChange()
	{
		DEBUG_LINE();
	}
	
	void _onPaintEvent(const spk::PaintEvent& p_event)
	{
		DEBUG_LINE();
	}
	
	void _onUpdateEvent(const spk::UpdateEvent& p_event)
	{
		DEBUG_LINE();
	}
	
	void _onKeyboardEvent(const spk::KeyboardEvent& p_event)
	{
		DEBUG_LINE();
	}
	
	void _onMouseEvent(const spk::MouseEvent& p_event)
	{

	}
	
	void _onControllerEvent(const spk::ControllerEvent& p_event)
	{
		DEBUG_LINE();
	}

public:
	TestWidget(spk::SafePointer<Widget> p_parent) :
		spk::Widget(L"TestWidget", p_parent),
		_pipeline(spk::JSON::File("shader/shader.json"))
	{
		
	}
};

int main()
{
	spk::GraphicalApplication app;
	
	spk::SafePointer<spk::Window> window = app.createWindow(L"Playground", spk::Geometry2DInt({100, 100}, {800, 800}));

	TestWidget testWidget(window->widget());
	
	testWidget.setGeometry(spk::Geometry2DInt({ 0, 0 }, { 800, 800 }));
	testWidget.activate();

	return app.run();
}

