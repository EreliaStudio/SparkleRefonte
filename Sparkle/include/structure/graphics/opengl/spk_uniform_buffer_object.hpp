#pragma once

#include <unordered_map>
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

        class Factory;

        class Layout
        {
            friend class Factory;
        public:
            struct DataRepresentation
            {
                size_t offset;
                size_t size;
            };

        private:
            char* _destination = nullptr; //The buffer where to write the data
            DataRepresentation _gpu;
            DataRepresentation _cpu;
            bool _tightlyPacked = false; //Can this layout be pushed at once or should be splited into multiples calls

            std::unordered_map<std::wstring, Layout> _innerLayouts;

            void _pushData(const char* p_basePtr)
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

            Layout(const DataRepresentation& p_cpu, const DataRepresentation& p_gpu) :
                _cpu(p_cpu),
                _gpu(p_gpu),
                _tightlyPacked(p_cpu.size == p_gpu.size)
            {

            }

        public:
            Layout() = default;

            void bind(char* p_destination)
            {
                _destination = p_destination;
                for (auto& [key, member] : _innerLayouts)
                {
                    member.bind(p_destination + member._gpu.offset);
                }
            }

            template <typename TType>
            Layout& operator = (const TType& p_data)
            {
                if (sizeof(TType) != _cpu.size)
                    throw std::invalid_argument("Unexpected parameter type provided to uniform buffer object.\nExpected [" + std::to_string(_cpu.size) + "] byte(s), receive [" + std::to_string(sizeof(TType)) + "].");

                _pushData(reinterpret_cast<const char*>(&p_data));
                return *this;
            }

            Layout& operator[](const std::wstring& p_name)
            {
                if (_innerLayouts.contains(p_name) == false)
                    throw std::runtime_error("No layout named [" + spk::StringUtils::wstringToString(p_name) + "] found.");
                return (_innerLayouts[p_name]);
            }
        };

        class Factory
        {
        private:
            std::string _typeName = "";
            BindingPoint _bindingPoint = -1;
            Layout _layout;

        public:
            void setTypeName(const std::string& p_name)
            {
                _typeName = p_name;
            }
            void setBindingPoint(BindingPoint p_bindingPoint)
            {
                _bindingPoint = p_bindingPoint;
            }
            void addInnerLayout(Layout& p_layout, const std::wstring& p_name, const Layout::DataRepresentation& p_cpu, const Layout::DataRepresentation& p_gpu)
            {
                p_layout._innerLayouts[p_name] = Layout(p_cpu, p_gpu);
            }

            UniformBufferObject construct()
            {
                UniformBufferObject result;

                result._typeName = _typeName;
                result._bindingPoint = _bindingPoint;
                result._layout = _layout;
                result.resize(result._layout._cpu.size);
                result._layout.bind(static_cast<char *>(result.data()));

                return (result);
            }
        };

    private:        
        std::string _typeName = "";
        BindingPoint _bindingPoint = -1;
        BlockIndex _blockIndex = GL_INVALID_ENUM;
        Layout _layout;

    public:
        UniformBufferObject() :
            VertexBufferObject(VertexBufferObject::Type::Uniform, VertexBufferObject::Usage::Static)
        {

        }

        UniformBufferObject(UniformBufferObject&& p_other) noexcept :
            VertexBufferObject(std::move(p_other)),
            _bindingPoint(p_other._bindingPoint),
            _blockIndex(p_other._blockIndex),
            _typeName(std::move(p_other._typeName))
        {
            p_other._bindingPoint = 0;
            p_other._blockIndex = 0;
        }

        UniformBufferObject& operator=(UniformBufferObject&& p_other) noexcept
        {
            if (this != &p_other)
            {
                VertexBufferObject::operator=(std::move(p_other));

                _bindingPoint = p_other._bindingPoint;
                _blockIndex = p_other._blockIndex;
                _typeName = std::move(p_other._typeName);

                p_other._bindingPoint = 0;
                p_other._blockIndex = 0;
            }
            return *this;
        }

        void activate()
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

        template <typename TType>
        UniformBufferObject& operator=(const TType& p_layout)
        {
            _layout = p_layout;
            validate();
            return *this;
        }

        Layout& operator[](const std::wstring& p_name)
        {
            return (_layout[p_name]);
        }
    };
}
