#include "structure/graphics/spk_pipeline.hpp"

#include "utils/spk_string_utils.hpp"

namespace spk
{
	void Pipeline::Object::_activate()
	{
		_inputBufferSet.activate();
		_outputBufferSet.activate();

		for (auto& [name, attribute] : _attributes)
		{
			attribute.activate();
		}

		for (auto& [name, sampler] : _samplers)
		{
			sampler.activate();
		}
	}

	void Pipeline::Object::_deactivate()
	{
		_inputBufferSet.deactivate();
		_outputBufferSet.deactivate();

		for (auto& [name, attribute] : _attributes)
		{
			attribute.deactivate();
		}
	}

	OpenGL::LayoutBufferObject& Pipeline::Object::inputLayout()
	{
		return _inputBufferSet.layout();
	}

	OpenGL::LayoutBufferObject& Pipeline::Object::outputLayout()
	{
		return _outputBufferSet.layout();
	}

	OpenGL::IndexBufferObject& Pipeline::Object::indexes()
	{
		return _inputBufferSet.indexes();
	}

	Pipeline::Object::Attribute& Pipeline::Object::attribute(const std::wstring& p_name)
	{
		if (_attributes.find(p_name) == _attributes.end())
		{
			throw std::runtime_error("Attribute [" + StringUtils::wstringToString(p_name) + "] not found inside Object");
		}
		return _attributes[p_name];
	}

	Pipeline::Object::Sampler2D& Pipeline::Object::sampler2D(const std::wstring& p_name)
	{
		if (_samplers.find(p_name) == _samplers.end())
		{
			throw std::runtime_error("Sampler2D [" + StringUtils::wstringToString(p_name) + "] not found inside Object");
		}
		return _samplers[p_name];
	}

	void Pipeline::Object::render()
	{
		if (_owner == nullptr)
		{
			return;
		}

		_owner->_render(*this);
	}

	void Pipeline::_render(Object& p_object)
	{
		_program.activate();

		p_object._activate();

		_program.render(static_cast<GLsizei>(p_object.indexes().nbIndexes()));

		p_object._deactivate();

		_program.deactivate();
	}

	Pipeline::Pipeline(const std::filesystem::path& p_shaderFile)
		: _parser(FileUtils::readFileAsString(p_shaderFile)),
		_program(_parser.getVertexShaderCode(), _parser.getFragmentShaderCode())
	{
		// The _program will be loaded when _render is called
	}

	Pipeline::Object Pipeline::createObject()
	{
		Object result;

		result._owner = this;
		result._inputBufferSet = _parser.getLayoutFactory().construct(OpenGL::BufferSet::Direction::In);
		result._outputBufferSet = _parser.getLayoutFactory().construct(OpenGL::BufferSet::Direction::Out);
		for (const auto& [key, factory] : _parser.getAttributeFactories())
		{
			result._attributes[key] = factory.construct();
		}
		for (const auto& [key, factory] : _parser.getSamplerFactories())
		{
			result._samplers[key] = factory.construct();
		}

		return result;
	}
}
