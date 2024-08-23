#include "sparkle.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

namespace spk::OpenGL
{
	class VertexBufferObject
	{
	public:
		enum class Type : GLenum
		{
			Unknow = GL_INVALID_ENUM,
			Storage = GL_ARRAY_BUFFER,
			Layout = GL_ELEMENT_ARRAY_BUFFER,
			Uniform = GL_UNIFORM_BUFFER,
			Texture = GL_TEXTURE_BUFFER,
			TransformFeedback = GL_TRANSFORM_FEEDBACK_BUFFER,
			ShaderStorage = GL_SHADER_STORAGE_BUFFER,
			PixelPack = GL_PIXEL_PACK_BUFFER,
			PixelUnpack = GL_PIXEL_UNPACK_BUFFER,
			DrawIndirect = GL_DRAW_INDIRECT_BUFFER,
			AtomicCounter = GL_ATOMIC_COUNTER_BUFFER
		};

		enum class Usage : GLenum
		{
			Unknow = GL_INVALID_ENUM,
			Static = GL_STATIC_DRAW,
			Dynamic = GL_DYNAMIC_DRAW,
			Stream = GL_STREAM_DRAW,
			StaticRead = GL_STATIC_READ,
			DynamicRead = GL_DYNAMIC_READ,
			StreamRead = GL_STREAM_READ,
			StaticCopy = GL_STATIC_COPY,
			DynamicCopy = GL_DYNAMIC_COPY,
			StreamCopy = GL_STREAM_COPY
		};

	private:
		bool _validated = false;
		GLuint _id = 0;
		Type _type = Type::Unknow;
		Usage _usage = Usage::Unknow;

		spk::DataBuffer _buffer;
		size_t _currentBufferMaxSize = 0;

		void _allocate()
		{
			//Allocate the _id on the gpu
		}

		void _release()
		{
			//Release the _id
		}

		void _update()
		{
			const void* data = _buffer.data();
			size_t size = _buffer.size();

			if (_buffer.size() >= _currentBufferMaxSize)
			{
				//Resend the buffer data to the GPU using _id, _type, _usage, data and size
				_currentBufferMaxSize = _buffer.size();
			}
			else
			{
				//Simply update the buffer using _id, the type and usage
			}
		}

	public:
		VertexBufferObject()
		{

		}

		VertexBufferObject(Type p_type, Usage p_usage) :
			_type(p_type),
			_usage(p_usage)
		{

		}

		~VertexBufferObject()
		{
			if (true)/*Add a check for OpenGLContext active*/
			{
				if (_id != 0)
				{
					_release();
				}
			}
		}

		VertexBufferObject(const VertexBufferObject& p_other) :
			_type(p_other._type),
			_usage(p_other._usage)
		{
			edit(p_other._buffer.data(), p_other._buffer.size());
			validate();
		}

		 VertexBufferObject(VertexBufferObject&& p_other) noexcept :
			_validated(p_other._validated),
			_id(p_other._id),
			_type(p_other._type),
			_usage(p_other._usage),
			_buffer(std::move(p_other._buffer)),
			_currentBufferMaxSize(p_other._currentBufferMaxSize)
		{
			p_other._validated = false;
			p_other._id = 0;
			p_other._type = Type::Unknow;
			p_other._usage = Usage::Unknow;
			p_other._buffer.clear();
			p_other._currentBufferMaxSize = 0;
		}

		VertexBufferObject& operator=(const VertexBufferObject& p_other)
		{
			if (this != &p_other)
			{
				if (_id != 0)
				{
					_release();
				}

				_type = p_other._type;
				_usage = p_other._usage;
				_buffer = p_other._buffer;
				_currentBufferMaxSize = p_other._currentBufferMaxSize;
				_validated = p_other._validated;

				edit(p_other._buffer.data(), p_other._buffer.size());
				validate();
			}

			return *this;
		}

		VertexBufferObject& operator=(VertexBufferObject&& p_other) noexcept
		{
			if (this != &p_other)
			{
				if (_id != 0)
				{
					_release();
				}

				_validated = p_other._validated;
				_id = p_other._id;
				_type = p_other._type;
				_usage = p_other._usage;
				_buffer = std::move(p_other._buffer);
				_currentBufferMaxSize = p_other._currentBufferMaxSize;

				p_other._validated = false;
				p_other._id = 0;
				p_other._type = Type::Unknow;
				p_other._usage = Usage::Unknow;
				p_other._buffer.clear();
				p_other._currentBufferMaxSize = 0;
			}

			return *this;
		}

		virtual void activate()
		{
			if (_validated == true)
			{
				if (_id == 0)
				{
					_allocate();
				}
				_update();
			}

			//Bind the _id
		}

		virtual void deactivate()
		{
			//Bind the id 0 to deactivate this buffer
		}

		void resize(size_t p_size)
		{
			_buffer.resize(p_size);
		}

		void* data()
		{
			return (_buffer.data());
		}

		size_t size()
		{
			return (_buffer.size());
		}

		void edit(const void* p_data, size_t p_dataSize, size_t p_offset = 0)
		{
			if (_buffer.size() <= p_dataSize + p_offset)
				_buffer.resize(p_dataSize + p_offset);
			_buffer.edit(p_offset, p_data, p_dataSize);
		}

		void append(const void* p_data, size_t p_dataSize)
		{
			_buffer.append(p_data, p_dataSize);
		}

		void validate()
		{
			_validated = true;
		}
	};

	class VertexArrayObject
	{
	private:
		GLuint _id = 0;

		void _allocate()
		{
			//Allocate the _id on the gpu
		}

		void _release()
		{
			//Release the _id
		}

	public:
		VertexArrayObject()
		{

		}

		~VertexArrayObject()
		{
			if (true)/*Add a check for OpenGLContext active*/
			{
				if (_id != 0)
				{
					_release();
				}
			}
		}

		void activate()
		{
			if (_id == 0)
			{
				_allocate();
			}
			//Bind _id
		}

		void deactivate()
		{
			//Unbind _id
		}
	};

	class UniformBufferObject : public VertexBufferObject
	{
	public:
		using BindingPoint = int;
		using BlockIndex = int;

		class Layout
		{

		private:
			char *_dataOutput = nullptr;
			size_t _offset = 0;
			size_t _cppOffset = 0;
			size_t _size = 0;
			bool _tightlyPacked = false;
			std::map<std::wstring, Layout> _internalLayouts;

			static Layout _parseLayout(const spk::JSON::Object& p_layoutJson)
			{
				Layout result;

				result._offset = p_layoutJson[L"offset"].as<long>();
				result._cppOffset = p_layoutJson[L"cppOffset"].as<long>();
				result._size = p_layoutJson[L"size"].as<long>();
				result._tightlyPacked = p_layoutJson.contains(L"tightlypacked") ? p_layoutJson[L"tightlypacked"].as<bool>() : false;

				if (p_layoutJson.contains(L"members") == true)
				{
					const auto& jsonMembers = p_layoutJson[L"members"].members();
					for (const auto& [name, member] : jsonMembers)
					{
						result._internalLayouts[name] = _parseLayout((*member));
					}
				}

				return (result);
			}

			void _setDataOutput(char* p_dataOutput)
			{
				_dataOutput = p_dataOutput;
				for (auto& [key, member] : _internalLayouts)
				{
					member._setDataOutput(p_dataOutput);
				}
			}

			void _copyFrom(const char* p_basePtr)
			{
				if (_tightlyPacked)
				{
					std::memcpy(_dataOutput + _offset, p_basePtr + _cppOffset, _size);
				}
				else
				{
					for (auto& [name, member] : _internalLayouts)
					{
						member._copyFrom(p_basePtr);
					}
				}
			}

		public:
			Layout() = default;

			Layout(const spk::JSON::Object& p_layoutJson, VertexBufferObject& p_parentBuffer)
			{
				*this = _parseLayout(p_layoutJson);
				p_parentBuffer.resize(_size);
				_setDataOutput(static_cast<char *>(p_parentBuffer.data()));
			}

			Layout& operator[](const std::wstring& p_memberName)
			{
				auto it = _internalLayouts.find(p_memberName);
				if (it == _internalLayouts.end())
				{
					throw std::out_of_range("No member found with the name: " + spk::StringUtils::wstringToString(p_memberName));
				}
				return it->second;
			}

			template <typename TType>
			Layout& operator=(const TType& p_inputData)
			{
				_copyFrom(reinterpret_cast<const char*>(&p_inputData));
				return *this;
			}
		};

	private:
		BindingPoint _bindingPoint = -1;
		BlockIndex _blockIndex = -1;
		bool _validated = false;
		Layout _layout;

	public:
		UniformBufferObject() :
			VertexBufferObject(VertexBufferObject::Type::Uniform, VertexBufferObject::Usage::Static),
			_layout(spk::JSON::Object(), *this)
		{
		}

		UniformBufferObject(const spk::JSON::Object& layoutJson) :
			VertexBufferObject(VertexBufferObject::Type::Uniform, VertexBufferObject::Usage::Static),
			_layout(layoutJson, *this)
		{
		}

		UniformBufferObject(const UniformBufferObject& p_other) :
			VertexBufferObject(p_other),
			_bindingPoint(p_other._bindingPoint),
			_blockIndex(p_other._blockIndex),
			_validated(p_other._validated),
			_layout(p_other._layout)
		{
		}

		void activate() override
		{
			VertexBufferObject::activate();

		}

		template <typename TType>
		UniformBufferObject& operator=(const TType& p_layout)
		{
			_layout = p_layout;
			validate();
			return *this;
		}

		Layout& operator[](const std::wstring& p_memberName)
		{
			return _layout[p_memberName];
		}
	};
}

class TestWidget : public spk::Widget
{
private:
	void _onGeometryChange()
	{
        
	}
	
	void _onPaintEvent(const spk::PaintEvent& p_event)
	{

	}

public:
	TestWidget(spk::SafePointer<Widget> p_parent) :
		spk::Widget(L"TestWidget", p_parent)
	{
		
	}
};

//int main()
//{
//	spk::GraphicalApplication app;
//	
//	spk::SafePointer<spk::Window> window = app.createWindow(L"Playground", spk::Geometry2DInt({100, 100}, {800, 800}));
//    
//	TestWidget testWidget = TestWidget(window->widget());
//	testWidget.setGeometry({0, 0, 800, 800});
//	testWidget.activate();
//
//	return app.run();
//}


int main()
{
	// JSON layout string representing the buffer layout
	std::wstring layoutJsonStr = LR"({
        "offset": 0,
        "cppOffset": 0,
        "size": 80,
        "tightlypacked": false,
        "members": {
			"transform": {
				"offset": 0,
				"cppOffset": 0,
				"size": 48,
				"tightlypacked": false,
				"members": {
						"position": {
							"offset": 0,
							"cppOffset": 0,
							"size": 12,
							"tightlypacked": true,
							"members": {}
						},
						"rotation": {
							"offset": 16,
							"cppOffset": 12,
							"size": 12,
							"tightlypacked": true,
							"members": {}
						},
						"scale": {
							"offset": 32,
							"cppOffset": 24,
							"size": 12,
							"tightlypacked": true,
							"members": {}
						}
					}
			},
			"color": {
				"offset": 64,
				"cppOffset": 48,
				"size": 16,
				"tightlypacked": true,
				"members": {}
			}
		}
    })";

	// Parse the JSON string into a spk::JSON::Object
	spk::JSON::File layoutJson = spk::JSON::File::loadFromString(layoutJsonStr);

	// Create the UniformBufferObject with the parsed layout
	spk::OpenGL::UniformBufferObject ubo(layoutJson.root());

	// Example data to write to the buffer
	spk::Vector3 position = { 1.0f, 2.0f, 3.0f };
	spk::Vector3 rotation = { 0.1f, 0.2f, 0.3f };
	spk::Vector3 scale = { 1.0f, 1.0f, 1.0f };
	spk::Color color = { 1.0f, 0.5f, 0.25f, 1.0f };

	// Assign the transform data to the buffer
	ubo[L"transform"][L"position"] = position;
	ubo[L"transform"][L"rotation"] = rotation;
	ubo[L"transform"][L"scale"] = scale;

	// Assign the color data to the buffer
	ubo[L"color"] = color;

	// Validate the buffer (ready to push to GPU)
	ubo.validate();

	// Activate the UBO (simulates binding it for GPU usage)
	ubo.activate();

	// Retrieve and print buffer contents for verification
	const auto& bufferData = ubo.data();

	std::cout << "Buffer contents as floats:" << std::endl;
	const float* floatData = reinterpret_cast<const float*>(bufferData);
	size_t floatCount = ubo.size() / sizeof(float);

	for (size_t i = 0; i < floatCount; ++i)
	{
		std::cout << floatData[i] << " ";
		if ((i + 1) % 4 == 0)
			std::cout << std::endl;
	}

	return 0;
}
