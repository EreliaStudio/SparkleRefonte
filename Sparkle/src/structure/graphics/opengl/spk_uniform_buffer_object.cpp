#include "structure/graphics/opengl/spk_uniform_buffer_object.hpp"

#include <GL/glew.h>
#include <windows.h>

#include "spk_debug_macro.hpp"

namespace spk::OpenGL
{
    UniformBufferObject::Layout UniformBufferObject::Layout::_parseLayout(const spk::JSON::Object& p_layoutJson)
    {
        Layout result;

        result._offset = p_layoutJson[L"Offset"].as<long>();
        result._cppOffset = p_layoutJson[L"CppOffset"].as<long>();
        result._size = p_layoutJson[L"Size"].as<long>();
        result._tightlyPacked = p_layoutJson[L"Tightlypacked"].as<bool>();

        if (p_layoutJson.contains(L"Members") == true)
        {
            const auto& jsonMembers = p_layoutJson[L"Members"].members();
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
        _layout(spk::JSON::Object(), *this),
        _typeName("")
    {
    }

    UniformBufferObject::UniformBufferObject(const spk::JSON::Object& p_layoutJson) :
        VertexBufferObject(VertexBufferObject::Type::Uniform, VertexBufferObject::Usage::Static),
        _layout(p_layoutJson[L"Composition"], *this),
        _typeName(spk::StringUtils::wstringToString(p_layoutJson[L"Type"].as<std::wstring>()))
    {
        _bindingPoint = p_layoutJson[L"BindingPoint"].as<long>();
    }

    UniformBufferObject::UniformBufferObject(const UniformBufferObject& p_other) :
        VertexBufferObject(p_other),
        _bindingPoint(p_other._bindingPoint),
        _blockIndex(p_other._blockIndex),
        _validated(p_other._validated),
        _layout(p_other._layout),
        _typeName(p_other._typeName)
    {
    }

    UniformBufferObject::UniformBufferObject(UniformBufferObject&& p_other) noexcept :
        VertexBufferObject(std::move(p_other)),
        _bindingPoint(p_other._bindingPoint),
        _blockIndex(p_other._blockIndex),
        _validated(p_other._validated),
        _layout(std::move(p_other._layout)),
        _typeName(std::move(p_other._typeName))
    {
        p_other._bindingPoint = 0;
        p_other._blockIndex = 0;
        p_other._validated = true;
    }

    UniformBufferObject& UniformBufferObject::operator=(const UniformBufferObject& p_other)
    {
        if (this != &p_other)
        {
            VertexBufferObject::operator=(p_other);

            _bindingPoint = p_other._bindingPoint;
            _blockIndex = p_other._blockIndex;
            _validated = p_other._validated;
            _layout = p_other._layout;
            _typeName = p_other._typeName;
        }
        return *this;
    }

    UniformBufferObject& UniformBufferObject::operator=(UniformBufferObject&& p_other) noexcept
    {
        if (this != &p_other)
        {
            if (_id != 0)
            {
                _release();
            }

            VertexBufferObject::operator=(std::move(p_other));

            _bindingPoint = p_other._bindingPoint;
            _blockIndex = p_other._blockIndex;
            _validated = p_other._validated;
            _layout = std::move(p_other._layout);
            _typeName = std::move(p_other._typeName);

            p_other._bindingPoint = 0;
            p_other._blockIndex = 0;
            p_other._validated = false;
        }
        return *this;
    }


    void UniformBufferObject::activate()
    {
        if (_blockIndex == GL_INVALID_ENUM)
        {
            GLint prog = 0;
            glGetIntegerv(GL_CURRENT_PROGRAM, &prog);

            _blockIndex = glGetUniformBlockIndex(prog, _typeName.c_str());

            glUniformBlockBinding(prog, _blockIndex, _bindingPoint);
        }
        VertexBufferObject::activate();
        glBindBufferBase(GL_UNIFORM_BUFFER, _bindingPoint, _id);

    }

    UniformBufferObject::Layout& UniformBufferObject::operator[](const std::wstring& p_memberName)
    {
        return _layout[p_memberName];
    }
}
