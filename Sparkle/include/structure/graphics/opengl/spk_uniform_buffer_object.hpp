#pragma once

#include <map>
#include <string>
#include "structure/graphics/opengl/spk_vertex_buffer_object.hpp"
#include "structure/container/spk_json_object.hpp"
#include "utils/spk_string_utils.hpp"

namespace spk::OpenGL
{
    class UniformBufferObject : public VertexBufferObject
    {
    public:
        using BindingPoint = int;
        using BlockIndex = int;

        class Layout
        {
        private:
            char* _dataOutput = nullptr;
            size_t _bindingPoint = 0;
            size_t _offset = 0;
            size_t _cppOffset = 0;
            size_t _size = 0;
            bool _tightlyPacked = false;
            std::map<std::wstring, Layout> _internalLayouts;

            static Layout _parseLayout(const spk::JSON::Object& p_layoutJson);
            void _setDataOutput(char* p_dataOutput);
            void _copyFrom(const char* p_basePtr);

        public:
            Layout() = default;

            Layout(const spk::JSON::Object& p_layoutJson, VertexBufferObject& p_parentBuffer);

            Layout& operator[](const std::wstring& p_memberName);

            template <typename TType>
            Layout& operator=(const TType& p_inputData)
            {
                if (sizeof(TType) != _size)
                {
                    throw std::runtime_error("Size mismatch in Uniform assignment. "
                        "Expected a size of " + std::to_string(_size) + " bytes, "
                        "but received " + std::to_string(sizeof(TType)) + " bytes of data.");
                }
                _copyFrom(reinterpret_cast<const char*>(&p_inputData));
                return *this;
            }
        };

    private:
        std::string _typeName = "";
        BindingPoint _bindingPoint = -1;
        BlockIndex _blockIndex = GL_INVALID_ENUM;
        bool _validated = false;
        Layout _layout;

    public:
        UniformBufferObject();
        UniformBufferObject(const spk::JSON::Object& layoutJson);
        UniformBufferObject(const UniformBufferObject& p_other);
        UniformBufferObject(UniformBufferObject&& p_other) noexcept;

        UniformBufferObject& operator=(const UniformBufferObject& p_other);
        UniformBufferObject& operator=(UniformBufferObject&& p_other) noexcept;

        void activate() override;

        template <typename TType>
        UniformBufferObject& operator=(const TType& p_layout)
        {
            _layout = p_layout;
            validate();
            return *this;
        }

        Layout& operator[](const std::wstring& p_memberName);
    };
}