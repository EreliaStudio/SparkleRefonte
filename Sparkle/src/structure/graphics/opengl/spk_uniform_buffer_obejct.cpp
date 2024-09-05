#include "structure/graphics/opengl/spk_uniform_buffer_object.hpp"

#include "utils/spk_string_utils.hpp"

namespace spk::OpenGL
{
	UniformBufferObject::Layout::Layout()
		: _destination(nullptr), _gpu{ 0, 0 }, _cpu{ 0, 0 }, _tightlyPacked(false) {}

	UniformBufferObject::Layout::Layout(const DataRepresentation& p_cpu, const DataRepresentation& p_gpu)
		: _cpu(p_cpu), _gpu(p_gpu), _tightlyPacked(p_cpu.size == p_gpu.size), _destination(nullptr) {}

	void UniformBufferObject::Layout::bind(char* p_destination)
	{
		_destination = p_destination;
		for (auto& [key, member] : _innerLayouts)
		{
			member.bind(p_destination + member._gpu.offset);
		}
	}

	template <typename TType>
	UniformBufferObject::Layout& UniformBufferObject::Layout::operator=(const TType& p_data)
	{
		if (sizeof(TType) != _cpu.size)
			throw std::invalid_argument("Unexpected parameter type provided to uniform buffer object.\nExpected [" + std::to_string(_cpu.size) + "] byte(s), received [" + std::to_string(sizeof(TType)) + "].");

		_pushData(reinterpret_cast<const char*>(&p_data));
		return *this;
	}

	UniformBufferObject::Layout& UniformBufferObject::Layout::operator[](const std::wstring& p_name)
	{
		if (_innerLayouts.find(p_name) == _innerLayouts.end())
			throw std::runtime_error("No layout named [" + spk::StringUtils::wstringToString(p_name) + "] found.");
		return _innerLayouts[p_name];
	}

	void UniformBufferObject::Layout::_pushData(const char* p_basePtr)
	{
		if (_tightlyPacked)
		{
			std::memcpy(_destination, p_basePtr, _cpu.size);
		}
		else
		{
			for (auto& [name, member] : _innerLayouts)
			{
				member._pushData(p_basePtr + member._cpu.offset);
			}
		}
	}

	void UniformBufferObject::Factory::setTypeName(const std::string& p_name)
	{
		_typeName = p_name;
	}

	void UniformBufferObject::Factory::setBindingPoint(BindingPoint p_bindingPoint)
	{
		_bindingPoint = p_bindingPoint;
	}

	const std::string& UniformBufferObject::Factory::typeName() const
	{
		return (_typeName);
	}

	UniformBufferObject::BindingPoint UniformBufferObject::Factory::bindingPoint() const
	{
		return (_bindingPoint);
	}

	UniformBufferObject::Layout& UniformBufferObject::Factory::mainLayout()
	{
		return _layout;
	}

	const UniformBufferObject::Layout& UniformBufferObject::Factory::mainLayout() const
	{
		return _layout;
	}

	UniformBufferObject::Layout& UniformBufferObject::Factory::addInnerLayout(Layout& p_layout, const std::wstring& p_name, const Layout::DataRepresentation& p_cpu, const Layout::DataRepresentation& p_gpu)
	{
		p_layout._innerLayouts[p_name] = Layout(p_cpu, p_gpu);
		return p_layout._innerLayouts[p_name];
	}

	UniformBufferObject UniformBufferObject::Factory::construct() const
	{
		UniformBufferObject result;
		result._typeName = _typeName;
		result._bindingPoint = _bindingPoint;
		result._layout = _layout;
		result.resize(result._layout._cpu.size);
		result._layout.bind(static_cast<char*>(result.data()));

		return result;
	}

	UniformBufferObject::UniformBufferObject()
		: VertexBufferObject(VertexBufferObject::Type::Uniform, VertexBufferObject::Usage::Static), _bindingPoint(-1), _blockIndex(GL_INVALID_ENUM) {}

	UniformBufferObject::UniformBufferObject(UniformBufferObject&& p_other) noexcept
		: VertexBufferObject(std::move(p_other)), _bindingPoint(p_other._bindingPoint), _blockIndex(p_other._blockIndex), _typeName(std::move(p_other._typeName)), _layout(std::move(p_other._layout))
	{
		p_other._bindingPoint = -1;
		p_other._blockIndex = GL_INVALID_ENUM;
	}

	UniformBufferObject& UniformBufferObject::operator=(UniformBufferObject&& p_other) noexcept
	{
		if (this != &p_other)
		{
			VertexBufferObject::operator=(std::move(p_other));

			_bindingPoint = p_other._bindingPoint;
			_blockIndex = p_other._blockIndex;
			_typeName = std::move(p_other._typeName);
			_layout = std::move(p_other._layout);

			p_other._bindingPoint = -1;
			p_other._blockIndex = GL_INVALID_ENUM;
		}
		return *this;
	}

	const std::string& UniformBufferObject::typeName() const
	{
		return _typeName;
	}

	const UniformBufferObject::BindingPoint& UniformBufferObject::bindingPoint() const
	{
		return _bindingPoint;
	}

	void UniformBufferObject::activate()
	{
		VertexBufferObject::activate();

		if (_blockIndex == GL_INVALID_ENUM)
		{
			GLint prog = 0;
			glGetIntegerv(GL_CURRENT_PROGRAM, &prog);

			_blockIndex = glGetUniformBlockIndex(prog, _typeName.c_str());

			glUniformBlockBinding(prog, _blockIndex, _bindingPoint);
		}

		glBindBufferBase(GL_UNIFORM_BUFFER, _bindingPoint, _id);
	}

	UniformBufferObject::Layout& UniformBufferObject::operator[](const std::wstring& p_name)
	{
		return _layout[p_name];
	}
}
