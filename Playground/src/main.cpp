#include "sparkle.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

#include <span>

#include "utils/spk_string_utils.hpp"
#include "utils/spk_opengl_utils.hpp"

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

        struct Attribute
        {
            enum class Type
            {
                Float,           // 1 component, GL_FLOAT
                Int,             // 1 component, GL_INT
                UInt,            // 1 component, GL_UNSIGNED_INT
                Vector2,         // 2 components, GL_FLOAT
                Vector2Int,      // 2 components, GL_INT
                Vector2UInt,     // 2 components, GL_UNSIGNED_INT
                Vector3,         // 3 components, GL_FLOAT
                Vector3Int,      // 3 components, GL_INT
                Vector3UInt,     // 3 components, GL_UNSIGNED_INT
                Vector4,         // 4 components, GL_FLOAT
                Vector4Int,      // 4 components, GL_INT
                Vector4UInt      // 4 components, GL_UNSIGNED_INT
            };

            static Type typeFromString(const std::wstring& p_input)
            {
                if (p_input == L"Float")
                {
                    return Type::Float;
                }
                else if (p_input == L"Int")
                {
                    return Type::Int;
                }
                else if (p_input == L"UInt")
                {
                    return Type::UInt;
                }
                else if (p_input == L"Vector2")
                {
                    return Type::Vector2;
                }
                else if (p_input == L"Vector2Int")
                {
                    return Type::Vector2Int;
                }
                else if (p_input == L"Vector2UInt")
                {
                    return Type::Vector2UInt;
                }
                else if (p_input == L"Vector3")
                {
                    return Type::Vector3;
                }
                else if (p_input == L"Vector3Int")
                {
                    return Type::Vector3Int;
                }
                else if (p_input == L"Vector3UInt")
                {
                    return Type::Vector3UInt;
                }
                else if (p_input == L"Vector4")
                {
                    return Type::Vector4;
                }
                else if (p_input == L"Vector4Int")
                {
                    return Type::Vector4Int;
                }
                else if (p_input == L"Vector4UInt")
                {
                    return Type::Vector4UInt;
                }
                else
                {
                    throw std::invalid_argument("Invalid input string [" + spk::StringUtils::wstringToString(p_input) + "] for Type.");
                }
            }

            GLsizei index;
            GLint size;
            GLenum type;
        };

    private:
        GLuint _id;
        Type _type;
        Usage _usage;
        GLsizei _stride;
        bool _needAttributeInitialization;
        std::vector<Attribute> _attributes;
        GLsizeiptr _currentSize;
        spk::DataBuffer _datas;
        bool _needsUpdate;
        mutable std::mutex _mutex;

    public:
        VertexBufferObject(Type p_type, Usage p_usage) :
            _type(p_type),
            _usage(p_usage),
            _currentSize(0),
            _needsUpdate(false),
            _stride(0),
			_needAttributeInitialization(false)
        {

        }

        VertexBufferObject(const VertexBufferObject& other) :
            _type(other._type),
            _usage(other._usage),
            _currentSize(other._currentSize),
            _datas(other._datas),
            _needsUpdate(true),
            _stride(other._stride),
            _attributes(other._attributes),
			_needAttributeInitialization(true)
        {
            
        }

        VertexBufferObject(VertexBufferObject&& other) noexcept :
            _id(other._id),
            _type(other._type),
            _usage(other._usage),
            _currentSize(other._currentSize),
            _datas(std::move(other._datas)),
            _needsUpdate(other._needsUpdate),
            _stride(other._stride),
			_attributes(other._attributes),
			_needAttributeInitialization(other._needAttributeInitialization)
        {
            other._id = 0;
        }

        ~VertexBufferObject()
        {
            if (_id != 0)
                release();
        }

        void allocate()
        {
			std::lock_guard<std::mutex> lock(_mutex);
            glGenBuffers(1, &_id);
			spk::cout << "Allocate VBO : " << _id << std::endl;
        }

        void release()
        {
			spk::cout << "Release VBO : " << _id << std::endl;
            glDeleteBuffers(1, &_id);
            _id = 0;
        }

        VertexBufferObject& operator=(const VertexBufferObject& other)
        {
            if (this != &other)
            {
                if (_id != 0)
                    release();

                _type = other._type;
                _usage = other._usage;
                _stride = other._stride;
				_needsUpdate = true;
				_needAttributeInitialization = true;

                allocate();
				push(other._datas);
            }
            return *this;
        }

        VertexBufferObject& operator=(VertexBufferObject&& other) noexcept
        {
            if (this != &other)
            {
                if (_id != 0)
                    release();

                _id = other._id;
                _type = other._type;
                _usage = other._usage;
                _currentSize = other._currentSize;
                _datas = std::move(other._datas);
				_needsUpdate = other._needsUpdate;
				_needAttributeInitialization = other._needAttributeInitialization;
                _stride = other._stride;

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

        size_t stride() const
        {
            return (_stride);
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

        void activate()
        {
            if (_id == 0)
                allocate();

			std::lock_guard<std::mutex> lock(_mutex);
			spk::cout << "Activate VBO [" << _id << "]" << std::endl;
            glBindBuffer(static_cast<GLenum>(_type), _id);

			if (_needAttributeInitialization == true)
			{
				size_t cumulatedSize = 0;
				for (const auto& attribute : _attributes)
				{
					spk::cout << "glVertexAttribPointer(" << attribute.index << ", " << attribute.size << ", " << spk::OpenGLUtils::to_wstring(attribute.type) << ", GL_FALSE, " << _stride << ", " << cumulatedSize << ")" << std::endl;
					glVertexAttribPointer(
						static_cast<GLuint>(attribute.index),
						attribute.size,
						attribute.type,
						GL_FALSE,
						_stride,
						(void*)(cumulatedSize)
					);

					spk::cout << "glEnableVertexAttribArray(" << attribute.index << ")" << std::endl;
					glEnableVertexAttribArray(static_cast<GLuint>(attribute.index));
					cumulatedSize += attribute.size * sizeof(float);
				}
				_needAttributeInitialization = false;
			}
        }

        void deactivate() const
        {
            std::lock_guard<std::mutex> lock(_mutex);
			spk::cout << "Deactivate VBO [" << _id << "]" << std::endl;
            glBindBuffer(static_cast<GLenum>(_type), 0);
        }

        void setupStride(size_t p_stride)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _stride = static_cast<GLsizei>(p_stride);  // Save the desired stride
        }

        void setupAttribute(const size_t& p_index, Attribute::Type p_type)
        {
            GLint size = 0;
            GLenum type = GL_FLOAT;

            switch (p_type)
            {
            case Attribute::Type::Float:
            case Attribute::Type::Int:
            case Attribute::Type::UInt:
                size = 1;
                break;

            case Attribute::Type::Vector2:
            case Attribute::Type::Vector2Int:
            case Attribute::Type::Vector2UInt:
                size = 2;
                break;

            case Attribute::Type::Vector3:
            case Attribute::Type::Vector3Int:
            case Attribute::Type::Vector3UInt:
                size = 3;
                break;

            case Attribute::Type::Vector4:
            case Attribute::Type::Vector4Int:
            case Attribute::Type::Vector4UInt:
                size = 4;
                break;

            default:
                throw std::invalid_argument("Unknown Attribute::Type");
            }

            switch (p_type)
            {
            case Attribute::Type::Float:
            case Attribute::Type::Vector2:
            case Attribute::Type::Vector3:
            case Attribute::Type::Vector4:
                type = GL_FLOAT;
                break;

            case Attribute::Type::Int:
            case Attribute::Type::Vector2Int:
            case Attribute::Type::Vector3Int:
            case Attribute::Type::Vector4Int:
                type = GL_INT;
                break;

            case Attribute::Type::UInt:
            case Attribute::Type::Vector2UInt:
            case Attribute::Type::Vector3UInt:
            case Attribute::Type::Vector4UInt:
                type = GL_UNSIGNED_INT;
                break;

            default:
                throw std::invalid_argument("Unknown Attribute::Type");
            }

            _attributes.push_back({
                    static_cast<GLsizei>(p_index),
                    static_cast<GLint>(size),
                    static_cast<GLenum>(type)
                });
            _needAttributeInitialization = true;
        }

        void* map(GLenum access)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            activate();
            void* ptr = glMapBuffer(static_cast<GLenum>(_type), access);
            deactivate();
            return ptr;
        }

        void unmap()
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
			_needsUpdate = true;
        }

        template <typename TType>
        void push(const TType& p_data, GLsizeiptr p_offset = 0)
        {
            push(&p_data, sizeof(TType), p_offset);
        }

        template <typename TType>
        void push(std::span<TType> p_data, GLsizeiptr p_offset = 0)
        {
            push(p_data.data(), static_cast<GLsizeiptr>(p_data.size_bytes()), p_offset);
        }
        
        template <typename TType>
        void push(const std::vector<TType>& p_data, GLsizeiptr p_offset = 0)
        {
            push(p_data.data(), static_cast<GLsizeiptr>(p_data.size() * sizeof(TType)), p_offset);
        }

        void append(const void* p_data, GLsizeiptr p_size)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _datas.append(p_data, p_size);
			_needsUpdate = true;
        }

        template <typename TType>
        void append(const TType& p_data)
        {
            append(&p_data, sizeof(TType));
        }

        template <typename TType>
        void append(std::span<TType> p_data)
        {
            append(p_data.data(), static_cast<GLsizeiptr>(p_data.size_bytes()));
        }

        template <typename TType>
        void append(const std::vector<TType>& p_data)
        {
            append(p_data.data(), static_cast<GLsizeiptr>(p_data.size() * sizeof(TType)));
        }

        void validate()
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _needsUpdate = true;
        }

        void bind()
        {
            activate();

            std::lock_guard<std::mutex> lock(_mutex);

            if (_needsUpdate == true && _datas.size() > 0)
            {
				spk::cout << "Bind VBO [" << _id << "]" << std::endl;
                if (_datas.size() <= static_cast<size_t>(_currentSize))
                {
					spk::cout << "Updating up [" << _datas.size() << "] bytes of data" << std::endl;

                    glBufferSubData(static_cast<GLenum>(_type), 0, _datas.size(), _datas.data());
                }
                else
                {
					spk::cout << "Sending up [" << _datas.size() << "] bytes of data" << std::endl;

                    glBufferData(static_cast<GLenum>(_type), _datas.size(), _datas.data(), static_cast<GLenum>(_usage));
                    _currentSize = _datas.size();
                }

                _needsUpdate = false;
            }
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
            if (_id != 0)
            {
                release();
            }
        }

        void allocate()
        {
            glGenVertexArrays(1, &_id);
			spk::cout << "Allocate VAO [" << _id << "]" << std::endl;
        }

        void release()
        {
			spk::cout << "Release VAO [" << _id << "]" << std::endl;
            glDeleteVertexArrays(1, &_id);
            _id = 0;
        }

        VertexArrayObject& operator=(VertexArrayObject&& other) noexcept
        {
            if (this != &other)
            {
                if (_id != 0)
                {
                    release();
                }

                _id = other._id;
                _isBound = other._isBound;

                other._id = 0;
                other._isBound = false;
            }
            return *this;
        }

        void activate()
        {
            if (_id == 0)
                allocate();
			spk::cout << "Activate VAO [" << _id << "]" << std::endl;
            glBindVertexArray(_id);
        }

        void deactivate() const
        {
			spk::cout << "Deactivate VAO [" << _id << "]" << std::endl;
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
            _elementBuffer.setupStride(sizeof(GLuint));
        }

        void activate()
        {
			spk::cout << "Activate storage" << std::endl;
            _vao.activate();
            _layoutBuffer.bind();
            _elementBuffer.bind();
        }

        void deactivate()
        {
			spk::cout << "Deactivate storage" << std::endl;
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

        template <typename TDataType>
        void pushVertices(const std::vector<TDataType>& p_datas)
        {
			if (sizeof(TDataType) != _layoutBuffer.stride())
			{
				DEBUG_LINE();
                throw std::runtime_error("Unexpected stride size received in storage push data\nexpected a vertex of [" + std::to_string(_layoutBuffer.stride()) + "] byte(s) but received a vertex of [" + std::to_string(sizeof(TDataType)) + "] byte(s)");
			}
            _layoutBuffer.push(p_datas);
        }

        template <typename TDataType>
        void appendVertices(const TDataType& p_data)
        {
            if (sizeof(TDataType) != _layoutBuffer.stride())
                throw std::runtime_error("Unexpected stride size received in storage append data\nexpected a vertex of [" + std::to_string(_layoutBuffer.stride()) + "] byte(s) but received a vertex of [" + std::to_string(sizeof(TDataType)) + "] byte(s)");
            _layoutBuffer.append(p_data);
        }

        void pushIndexes(const std::vector<unsigned int>& p_indexes)
        {
            _elementBuffer.push(p_indexes);
        }

        void appendIndex(unsigned int p_index)
        {
            _elementBuffer.append(p_index);
        }

		size_t nbIndexes() const
		{
			return (_elementBuffer.size() / _elementBuffer.stride());
		}

		size_t nbTriangles() const
		{
			return (nbIndexes() / 3);
		}

		size_t nbVertices() const
		{
			return (_layoutBuffer.size() / (_layoutBuffer.stride()));
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
                throw std::runtime_error("Uniform element name not found: " + spk::StringUtils::wstringToString(p_name));
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

        void bind()
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
			spk::cout << "Rendering object" << std::endl;
			_owner->activate();

			_storage.activate();

			for (auto& [key, uniform] : _attributes)
			{
				uniform.bind();
			}

			spk::cout << "Drawing [" << _storage.nbTriangles() << "] triangles with [" << _storage.layoutBuffer().size() << "] bytes (" << _storage.nbVertices() << " vertices)" << std::endl;
			_owner->draw(_storage.nbIndexes());

			_storage.deactivate();

			_owner->deactivate();

			spk::cout << std::endl << std::endl;
		}

		void renderInstanced(size_t p_nbInstance)
		{
			_owner->activate();

			_storage.activate();
			for (auto& [key, uniform] : _attributes)
			{
				uniform.bind();
			}

			_owner->drawInstanced(_storage.nbTriangles(), p_nbInstance);

			_owner->deactivate();
		}
	};

private:
	std::wstring _name;

	bool _isLoaded;
	GLuint _id;

	std::string _vertexCode;
	std::string _fragmentCode;

	size_t _objectStride;
	std::vector<std::tuple<long, std::wstring>> _objectAttributes;

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

    void activate() const
    {
		spk::cout << "Activating program " << _id << std::endl;
        glUseProgram(_id);
    }

    void deactivate() const
    {
		spk::cout << "Deactivating program " << _id << std::endl;
		glUseProgram(0);
    }

    void draw(size_t p_nbTriangle)
    {
        for (auto& [key, uniform] : _constants)
        {
            uniform.bind();
        }

        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(p_nbTriangle), GL_UNSIGNED_INT, nullptr);
    }

    void drawInstanced(size_t p_nbTriangle, size_t p_nbInstance)
    {
        for (auto& [key, uniform] : _constants)
        {
            uniform.bind();
        }

        glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(p_nbTriangle), GL_UNSIGNED_INT, nullptr, static_cast<GLsizei>(p_nbInstance));
    }

public:
    Pipeline(const spk::JSON::File& p_inputFile) :
        _isLoaded(false)
    {
        _vertexCode = spk::StringUtils::wstringToString(p_inputFile[L"VertexShaderCode"].as<std::wstring>());
        _fragmentCode = spk::StringUtils::wstringToString(p_inputFile[L"FragmentShaderCode"].as<std::wstring>());

        const auto& pipelineFlowObject = p_inputFile[L"PipelineFlow"];
        _objectStride = pipelineFlowObject[L"Stride"].as<long>();
        const auto& attributeObjects = pipelineFlowObject[L"Attributes"];
        for (size_t i = 0; i < attributeObjects.size(); i++)
        {
            const auto& attributeObject = attributeObjects[i];
            _objectAttributes.push_back(std::make_tuple(
                    attributeObject[L"Index"].as<long>(),
                    attributeObject[L"Type"].as<std::wstring>()
                ));
        }
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

    Object createObject()
    {
        Object result(this);

        result.storage().layoutBuffer().setupStride(_objectStride);
        for (const auto& attribute : _objectAttributes)
        {
            result.storage().layoutBuffer().setupAttribute(std::get<0>(attribute), VertexBufferObject::Attribute::typeFromString(std::get<1>(attribute)));
        }

        return (result);
    }
};
std::wstring to_wstring(Pipeline::VertexBufferObject::Type type);
std::wstring to_wstring(Pipeline::VertexBufferObject::Usage usage);
}

namespace spk
{
    std::wstring to_wstring(Pipeline::VertexBufferObject::Attribute::Type p_type)
    {
        switch (p_type)
        {
        case Pipeline::VertexBufferObject::Attribute::Type::Float:
            return L"Float";
        case Pipeline::VertexBufferObject::Attribute::Type::Int:
            return L"Int";
        case Pipeline::VertexBufferObject::Attribute::Type::UInt:
            return L"UInt";
        case Pipeline::VertexBufferObject::Attribute::Type::Vector2:
            return L"Vector2";
        case Pipeline::VertexBufferObject::Attribute::Type::Vector2Int:
            return L"Vector2Int";
        case Pipeline::VertexBufferObject::Attribute::Type::Vector2UInt:
            return L"Vector2UInt";
        case Pipeline::VertexBufferObject::Attribute::Type::Vector3:
            return L"Vector3";
        case Pipeline::VertexBufferObject::Attribute::Type::Vector3Int:
            return L"Vector3Int";
        case Pipeline::VertexBufferObject::Attribute::Type::Vector3UInt:
            return L"Vector3UInt";
        case Pipeline::VertexBufferObject::Attribute::Type::Vector4:
            return L"Vector4";
        case Pipeline::VertexBufferObject::Attribute::Type::Vector4Int:
            return L"Vector4Int";
        case Pipeline::VertexBufferObject::Attribute::Type::Vector4UInt:
            return L"Vector4UInt";
        default:                 throw std::invalid_argument("Invalid Type enum value.");
        }
    }

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

std::wostream& operator<<(std::wostream& os, spk::Pipeline::VertexBufferObject::Attribute::Type type)
{
    os << spk::to_wstring(type);
    return os;
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
	static inline spk::Pipeline _pipeline = spk::Pipeline(spk::JSON::File("shader/shader.json"));
    spk::Pipeline::Object _renderingObject;
    struct Vertex
    {
        spk::Vector2 position;
        spk::Color color;
    };

	void _onGeometryChange()
	{
        //std::vector<Vertex> vertices = {
        //    {
        //        spk::Vector2(0, 1),
        //        spk::Color::red
        //    },
        //    {
        //        spk::Vector2(-1, -1),
        //        spk::Color::red
        //    },
        //    {
        //        spk::Vector2(1, -1),
        //        spk::Color::red
        //    }
        //};

        //std::vector<unsigned int> indexes = { 0, 1, 2 };

        //_renderingObject.storage().pushVertices(vertices);
        //_renderingObject.storage().pushIndexes(indexes);
	}
	
	void _onPaintEvent(const spk::PaintEvent& p_event)
	{
        //_renderingObject.render();
	}

public:
	TestWidget(spk::SafePointer<Widget> p_parent) :
		spk::Widget(L"TestWidget", p_parent),
        _renderingObject(_pipeline.createObject())
	{
		
	}
};

int main()
{
	spk::GraphicalApplication app;
	
	spk::SafePointer<spk::Window> window = app.createWindow(L"Playground", spk::Geometry2DInt({100, 100}, {800, 800}));
    window->setMainWidget<TestWidget>(window->widget());

	return app.run();
}

