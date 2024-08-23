#include "structure/graphics/opengl/spk_uniform_buffer_object.hpp"

#include <GL/glew.h>
#include <windows.h>

namespace spk::OpenGL
{
    UniformBufferObject::Layout UniformBufferObject::Layout::_parseLayout(const spk::JSON::Object& p_layoutJson)
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

        return result;
    }

    void UniformBufferObject::Layout::_setDataOutput(char* p_dataOutput)
    {
        _dataOutput = p_dataOutput;
        for (auto& [key, member] : _internalLayouts)
        {
            member._setDataOutput(p_dataOutput + member._offset);
        }
    }

    void UniformBufferObject::Layout::_copyFrom(const char* p_basePtr)
    {
        if (_tightlyPacked)
        {
            std::memcpy(_dataOutput, p_basePtr, _size);
        }
        else
        {
            for (auto& [name, member] : _internalLayouts)
            {
                member._copyFrom(p_basePtr + member._cppOffset);
            }
        }
    }

    UniformBufferObject::Layout::Layout(const spk::JSON::Object& p_layoutJson, VertexBufferObject& p_parentBuffer)
    {
        *this = _parseLayout(p_layoutJson);
        p_parentBuffer.resize(_size);
        _setDataOutput(static_cast<char*>(p_parentBuffer.data()));
    }

    UniformBufferObject::Layout& UniformBufferObject::Layout::operator[](const std::wstring& p_memberName)
    {
        auto it = _internalLayouts.find(p_memberName);
        if (it == _internalLayouts.end())
        {
            throw std::out_of_range("No member found with the name: " + spk::StringUtils::wstringToString(p_memberName));
        }
        return it->second;
    }

    UniformBufferObject::UniformBufferObject() :
        VertexBufferObject(VertexBufferObject::Type::Uniform, VertexBufferObject::Usage::Static),
        _layout(spk::JSON::Object(), *this)
    {
    }

    UniformBufferObject::UniformBufferObject(const spk::JSON::Object& layoutJson) :
        VertexBufferObject(VertexBufferObject::Type::Uniform, VertexBufferObject::Usage::Static),
        _layout(layoutJson, *this)
    {
    }

    UniformBufferObject::UniformBufferObject(const UniformBufferObject& p_other) :
        VertexBufferObject(p_other),
        _bindingPoint(p_other._bindingPoint),
        _blockIndex(p_other._blockIndex),
        _validated(p_other._validated),
        _layout(p_other._layout)
    {
    }

    void UniformBufferObject::activate()
    {
        VertexBufferObject::activate();

        if (_bindingPoint == -1)
        {
            GLint maxUniformBufferBindings = 0;
            glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxUniformBufferBindings);
            for (int i = 0; i < maxUniformBufferBindings; ++i)
            {
                GLint binding = 0;
                glGetIntegeri_v(GL_UNIFORM_BUFFER_BINDING, i, &binding);
                if (binding == 0)
                {
                    _bindingPoint = i;
                    break;
                }
            }
        }

        if (_blockIndex == -1)
        {
            _blockIndex = glGetUniformBlockIndex(glGetProgram(), "yourBlockName");
        }

        glBindBufferBase(GL_UNIFORM_BUFFER, _bindingPoint, _id);

        glUniformBlockBinding(glGetProgram(), _blockIndex, _bindingPoint);
    }

    UniformBufferObject::Layout& UniformBufferObject::operator[](const std::wstring& p_memberName)
    {
        return _layout[p_memberName];
    }
}
