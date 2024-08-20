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
        bool _needAttributeInitialization = true;
        std::vector<Attribute> _attributes;
        GLsizeiptr _currentSize;
        spk::DataBuffer _datas;
        bool _needsUpdate = false;
        mutable std::mutex _mutex;

    public:
        VertexBufferObject() :
            _type(Type::Storage),
            _usage(Usage::Static),
            _currentSize(0),
            _needsUpdate(false),
            _stride(0),
            _needAttributeInitialization(false)
        {

        }

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
        }

        void release()
        {
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

            glBindBuffer(static_cast<GLenum>(_type), _id);

			if (_needAttributeInitialization == true)
			{
				size_t cumulatedSize = 0;
				for (const auto& attribute : _attributes)
				{
					glVertexAttribPointer(
						static_cast<GLuint>(attribute.index),
						attribute.size,
						attribute.type,
						GL_FALSE,
						_stride,
						(void*)(cumulatedSize)
					);

					glEnableVertexAttribArray(static_cast<GLuint>(attribute.index));
                    cumulatedSize += attribute.size *sizeof(float);
				}
				_needAttributeInitialization = false;
			}
        }

        void deactivate() const
        {
            std::lock_guard<std::mutex> lock(_mutex);
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
        }

        void release()
        {
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
            _elementBuffer.setupStride(sizeof(GLuint));
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

        void validate()
        {
            _layoutBuffer.validate();
            _elementBuffer.validate();
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
            GLsizeiptr _gpuOffset;
            GLsizeiptr _size;
            GLsizeiptr _cpuOffset;
            VertexBufferObject* _vbo;
            std::unordered_map<std::wstring, Element> _elements;

        public:
            Element() = default;

            Element(const spk::JSON::Object& p_jsonObject, VertexBufferObject* p_vbo) :
                _vbo(p_vbo)
            {

                _gpuOffset = static_cast<GLint>(p_jsonObject[L"OffsetGPU"].as<long>());
                _cpuOffset = static_cast<GLsizeiptr>(p_jsonObject[L"OffsetCPU"].as<long>());
                _size = static_cast<GLsizeiptr>(p_jsonObject[L"Size"].as<long>());

                if (p_jsonObject.contains(L"Attributes") == true)
                {
                    const auto& attributes = p_jsonObject[L"Attributes"].asArray();

                    for (size_t i = 0; i < attributes.size(); i++)
                    {
                        const auto& attribute = attributes[i];
                        std::wstring elementName = attribute->operator[](L"Name").as<std::wstring>();

                        _elements.emplace(elementName, Element(*attribute, p_vbo));
                    }
                }
            }

            Element(const Element& p_other) = default;

            size_t size() const
            {
                return (_size);
            }

            template <typename TType>
            Element& operator=(const TType& p_data)
            {
                spk::cout << "Receive edition of value [" << p_data << "]" << std::endl;
                if (sizeof(TType) != _size)
                {
                    throw std::runtime_error("Size mismatch in Uniform::Element assignment. "
                        "Expected a size of " + std::to_string(_size) + " bytes, "
                        "but received " + std::to_string(sizeof(TType)) + " bytes of data.");
                }

                if (_elements.empty() == false)
                {
                    const void* dataPtr = &p_data;
                    for (auto& [key, element] : _elements)
                    {
                        element.setDataFromBytes(dataPtr);
                    }
                }
                else
                {
                    const void* dataPtr = &p_data;
                    _vbo->push(dataPtr, _size, _gpuOffset);
                }
                return (*this);
            }

            Element& operator[](const std::wstring& p_name)
            {
                auto it = _elements.find(p_name);
                if (it == _elements.end())
                {
                    throw std::runtime_error("Uniform::Element element name not found: " + spk::StringUtils::wstringToString(p_name));
                }
                return it->second;
            }

            void setDataFromBytes(const void* p_basePtr)
            {
                if (_elements.empty() == false)
                {
                    for (auto& [key, element] : _elements)
                    {
                        element.setDataFromBytes(static_cast<const char*>(p_basePtr) + _cpuOffset);
                    }
                }
                else
                {
                    const void* dataPtr = static_cast<const char*>(p_basePtr) + _cpuOffset;
                    _vbo->push(dataPtr, _size, _gpuOffset);
                }
            }
        };

    private:
        VertexBufferObject _vbo;
        std::wstring _name;
        size_t _size;
        GLuint _bindingPoint;
        GLuint _blockIndex;
        std::unordered_map<std::wstring, Element> _elements;

    public:
        Uniform() :
            _vbo(VertexBufferObject::Type::Uniform, VertexBufferObject::Usage::Static),
            _bindingPoint(0),
            _blockIndex(0)
        {

        }

        Uniform(const spk::JSON::Object& p_jsonConfig, GLuint p_bindingPoint) :
            _vbo(VertexBufferObject::Type::Uniform, VertexBufferObject::Usage::Static),
            _bindingPoint(p_bindingPoint)
        {
            _name = p_jsonConfig[L"Type"].as<std::wstring>();
            _size = static_cast<GLsizeiptr>(p_jsonConfig[L"SizeCPU"].as<long>());

            GLsizeiptr bufferSize = static_cast<GLsizeiptr>(p_jsonConfig[L"SizeGPU"].as<long>());
            _vbo.reserve(bufferSize);
            _vbo.setupStride(bufferSize);

            const auto& attributes = p_jsonConfig[L"Attributes"].asArray();

            for (size_t i = 0; i < attributes.size(); i++)
            {
                const auto& attribute = attributes[i];
                std::wstring elementName = attribute->operator[](L"Name").as<std::wstring>();

                _elements[elementName] = std::move(Element(*attribute, &_vbo));
            }
        }

        Uniform duplicate() const
        {
            Uniform result;

            result._name = _name;
            result._size = _size;
            result._bindingPoint = _bindingPoint;
            result._blockIndex = _blockIndex;
            result._elements = _elements;

            return (result);
        }

        size_t size() const
        {
            return (_vbo.stride());
        }

        BindingPoint bindingPoint() const
        {
            return (_bindingPoint);
        }

        Element& operator[](const std::wstring& p_name)
        {
            if (_elements.contains(p_name) == false)
            {
                throw std::runtime_error("Uniform element name not found: " + spk::StringUtils::wstringToString(p_name));
            }
            return (_elements[p_name]);
        }

        template <typename TType>
        Uniform& operator=(const TType& p_data)
        {
            if (sizeof(TType) != _size)
            {
                throw std::runtime_error("Size mismatch in Uniform::Element assignment. "
                    "Expected a size of " + std::to_string(_size) + " bytes, "
                    "but received " + std::to_string(sizeof(TType)) + " bytes of data.");
            }

            const void* basePtr = &p_data;
            for (auto& [name, element] : _elements)
            {
                element.setDataFromBytes(basePtr);
            }
            return *this;
        }

        void bind()
        {
            if (_blockIndex == 0)
            {
                GLint program;
                glGetIntegerv(GL_CURRENT_PROGRAM, &program);

                GLuint blockIndex = glGetUniformBlockIndex(program, spk::StringUtils::wstringToString(_name).c_str());

                glUniformBlockBinding(program, blockIndex, _bindingPoint);
            }
            _vbo.bind();

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
        std::unordered_map<std::wstring, Uniform *> _attributes;

        Object(Pipeline* p_owner) :
            _owner(p_owner)
        {

        }

        void addAttribute(const std::wstring& p_name, Uniform* p_newAttribute)
        {
            if (_attributes.contains(p_name) == true)
            {
                throw std::runtime_error("Object already contains an attribute named [" + spk::StringUtils::wstringToString(p_name) + "]");
            }
            _attributes[p_name] = new Uniform(p_newAttribute->duplicate());
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

        std::unordered_map<std::wstring, Uniform*>& attributes()
        {
            return (_attributes);
        }

        const std::unordered_map<std::wstring, Uniform*>& attributes() const
        {
            return (_attributes);
        }

        Uniform& attribute(const std::wstring& p_name)
        {
            if (_attributes.contains(p_name) == false)
            {
                throw std::runtime_error("Object doesn't contains an attribute named [" + spk::StringUtils::wstringToString(p_name) + "]");
            }
            return (*_attributes[p_name]);
        }

        void render()
        {
			_owner->activate();

			_storage.activate();

			for (auto& [key, uniform] : _attributes)
			{
				uniform->bind();
			}

			_owner->draw(_storage.nbIndexes());

			_storage.deactivate();

			_owner->deactivate();
		}

		void renderInstanced(size_t p_nbInstance)
		{
			_owner->activate();

			_storage.activate();
			for (auto& [key, uniform] : _attributes)
			{
				uniform->bind();
			}

			_owner->drawInstanced(_storage.nbTriangles(), p_nbInstance);

			_owner->deactivate();
		}
	};

private:
	std::wstring _name;

	GLuint _id;

	std::string _vertexCode;
	std::string _fragmentCode;

	size_t _objectStride;
	std::vector<std::tuple<long, std::wstring>> _objectAttributes;

    static inline std::unordered_map<std::wstring, Uniform*> _constants;
    std::vector<Uniform*> _usedConstants;
    std::unordered_map<std::wstring, Uniform*> _attributeBlueprints;

    void applyUniformLocation(const std::string& p_uniformType, size_t p_bindingPoint)
    {
        const std::string searchPattern = ") uniform " + p_uniformType;
        const std::string insertPattern = ", binding = " + std::to_string(p_bindingPoint);

        // Function to update shader code by adding the binding point
        auto updateShaderCode = [&](std::string& shaderCode)
            {
                size_t pos = 0;
                while ((pos = shaderCode.find(searchPattern, pos)) != std::string::npos)
                {
                    // Move the position back to before the " uniform " to insert the binding point
                    pos = shaderCode.rfind("layout(", pos);
                    if (pos != std::string::npos)
                    {
                        size_t endPos = shaderCode.find(")", pos);
                        if (endPos != std::string::npos)
                        {
                            shaderCode.insert(endPos, insertPattern);
                        }
                    }
                    // Move past the current occurrence
                    pos += searchPattern.length();
                }
            };

        // Update both vertex and fragment shader code
        updateShaderCode(_vertexCode);
        updateShaderCode(_fragmentCode);
    }


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

    void load()
    {
        GLuint vertexShaderID = compileShader(_vertexCode, GL_VERTEX_SHADER);
        GLuint fragmentShaderID = compileShader(_fragmentCode, GL_FRAGMENT_SHADER);

        _id = linkShaders(vertexShaderID, fragmentShaderID);

        glDeleteShader(vertexShaderID);
        glDeleteShader(fragmentShaderID);
    }

    void activate()
    {
        if (_id == 0)
            load();

        glUseProgram(_id);
    }

    void deactivate()
    {
		glUseProgram(0);
    }

    void draw(size_t p_nbTriangle)
    {

        for (auto uniform : _usedConstants)
        {
            uniform->bind();
        }

        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(p_nbTriangle), GL_UNSIGNED_INT, nullptr);
    }

    void drawInstanced(size_t p_nbTriangle, size_t p_nbInstance)
    {
        for (auto uniform : _usedConstants)
        {
            uniform->bind();
        }

        glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(p_nbTriangle), GL_UNSIGNED_INT, nullptr, static_cast<GLsizei>(p_nbInstance));
    }

    void parsePipelineFlow(const spk::JSON::File& p_inputFile)
    {
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

    void parseConstants(const spk::JSON::File& p_inputFile)
    {
        const auto& constantsArray = p_inputFile[L"Constants"];
        for (size_t i = 0; i < constantsArray.size(); i++)
        {
            const auto& constantObject = constantsArray[i];
            std::wstring constantName = constantObject[L"Name"].as<std::wstring>();
            std::wstring constantType = constantObject[L"Type"].as<std::wstring>();
            GLsizeiptr jsonSize = constantObject[L"SizeGPU"].as<long>();

            auto it = _constants.find(constantName);
            if (it != _constants.end())
            {
                Uniform* existingUniform = it->second;
                GLsizeiptr existingSize = existingUniform->size();
                if (existingSize != jsonSize)
                {
                    throw std::runtime_error("Size mismatch for existing constant: " +
                        spk::StringUtils::wstringToString(constantName) +
                        ". Expected size: " + std::to_string(existingSize) +
                        ", JSON size: " + std::to_string(jsonSize));
                }

                _usedConstants.push_back(existingUniform);
            }
            else
            {
                GLuint bindingPoint = static_cast<GLuint>(_constants.size());

                _constants.emplace(constantName, new Uniform(constantObject, bindingPoint));

                _usedConstants.push_back(_constants[constantName]);
            }

            applyUniformLocation(spk::StringUtils::wstringToString(constantType), _constants[constantName]->bindingPoint());
        }
    }

    void parseAttributes(const spk::JSON::File& p_inputFile)
    {
        const auto& attributesArray = p_inputFile[L"Attributes"];
        for (size_t i = 0; i < attributesArray.size(); i++)
        {
            const auto& attributeObject = attributesArray[i];
            std::wstring attributeName = attributeObject[L"Name"].as<std::wstring>();
            std::wstring attributeType = attributeObject[L"Type"].as<std::wstring>();
            GLsizeiptr jsonSize = attributeObject[L"SizeGPU"].as<long>();

            GLuint bindingPoint = static_cast<GLuint>(_constants.size());

            _attributeBlueprints.emplace(attributeName, new Uniform(attributeObject, bindingPoint));

            applyUniformLocation(spk::StringUtils::wstringToString(attributeType), bindingPoint);
        }
    }

public:
    Pipeline(const spk::JSON::File& p_inputFile) :
        _id(0)
    {
        _vertexCode = spk::StringUtils::wstringToString(p_inputFile[L"VertexShaderCode"].as<std::wstring>());
        _fragmentCode = spk::StringUtils::wstringToString(p_inputFile[L"FragmentShaderCode"].as<std::wstring>());

        parsePipelineFlow(p_inputFile);

        parseConstants(p_inputFile);

        parseAttributes(p_inputFile);
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

        for (const auto& [key, attribute] : _attributeBlueprints)
        {
            result.addAttribute(key, attribute);
        }

        return (result);
    }

    Uniform& constant(const std::wstring& p_name)
    {
        if (_constants.contains(p_name) == false)
        {
            throw std::runtime_error("No uniform named [" + spk::StringUtils::wstringToString(p_name) + "] found");
        }
        return (*_constants.at(p_name));
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

        std::vector<unsigned int> indexes = { 0, 1, 2 };

        _renderingObject.storage().pushVertices(vertices);
        _renderingObject.storage().pushIndexes(indexes);
        _renderingObject.storage().validate();

        _renderingObject.attribute(L"colorBuffer") = spk::Color(255, 0, 0, 255);
        _renderingObject.attribute(L"colorBuffer").validate();
	}
	
	void _onPaintEvent(const spk::PaintEvent& p_event)
	{
        _renderingObject.render();
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

