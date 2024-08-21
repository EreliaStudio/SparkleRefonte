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
			Element = GL_ELEMENT_ARRAY_BUFFER,
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

	private:
		/*
		if (sizeof(TType) != _size)
				{
					throw std::runtime_error("Size mismatch in Uniform assignment. "
						"Expected a size of " + std::to_string(_size) + " bytes, "
						"but received " + std::to_string(sizeof(TType)) + " bytes of data.");
				}
		*/

		BindingPoint _bindingPoint = -1;
		BindingPoint _blockIndex = -1;

	public:
		UniformBufferObject() :
			VertexBufferObject(VertexBufferObject::Type::Uniform, VertexBufferObject::Usage::Static)
		{

		}

		UniformBufferObject(const UniformBufferObject& p_other) :
			_bindingPoint(p_other._bindingPoint),
			_blockIndex(p_other._blockIndex),
			_layout(p_other._layout)
		{

		}

		void activate() override
		{
			VertexBufferObject::activate();
			// Bind the UBO on the _bindingPoint and _blockIndex
		}

		template <typename TType>
		UniformBufferObject& operator = (const TType& p_data)
		{

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

int main()
{
	spk::GraphicalApplication app;
	
	spk::SafePointer<spk::Window> window = app.createWindow(L"Playground", spk::Geometry2DInt({100, 100}, {800, 800}));
    
	TestWidget testWidget = TestWidget(window->widget());
	testWidget.setGeometry({0, 0, 800, 800});
	testWidget.activate();

	return app.run();
}

