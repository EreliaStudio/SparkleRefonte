#pragma once

#include "structure/graphics/opengl/spk_vertex_array_object.hpp"
#include "structure/graphics/opengl/spk_index_buffer_object.hpp"
#include "structure/graphics/opengl/spk_layout_buffer_object.hpp"


namespace spk::OpenGL
{
    class BufferSet
    {
    public:
        class Factory
        {
        private:
            OpenGL::LayoutBufferObject::Factory _layoutFactory;

        public:
            void insert(OpenGL::LayoutBufferObject::Attribute::Index p_index, size_t p_size, OpenGL::LayoutBufferObject::Attribute::Type p_type)
            {
                _layoutFactory.insert(p_index, p_size, p_type);
            }

            BufferSet construct()
            {
                BufferSet result;

                result._layout = _layoutFactory.construct();

                return (result);
            }
        };

    private:
        VertexArrayObject _vao;
        LayoutBufferObject _layout;
        IndexBufferObject _indexes;

    public:
        BufferSet()
        {

        }

        LayoutBufferObject& layout()
        {
            return _layout;
        }
        IndexBufferObject& indexes()
        {
            return _indexes;
        }

        const LayoutBufferObject& layout() const
        {
            return _layout;
        }
        const IndexBufferObject& indexes() const
        {
            return _indexes;
        }

        void activate()
        {
            _vao.activate();
            _layout.activate();
            _indexes.activate();
        }
        void deactivate()
        {
            _vao.deactivate();
            _layout.deactivate();
            _indexes.deactivate();
        }
    };
}