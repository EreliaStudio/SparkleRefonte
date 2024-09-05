#pragma once

#include <unordered_map>
#include <string>
#include "spk_vertex_buffer_object.hpp"

namespace spk
{
	class ShaderParser;
}

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
			friend class spk::ShaderParser;

		public:
			struct DataRepresentation
			{
				size_t offset;
				size_t size;
			};

		public:
			Layout();
			Layout(const DataRepresentation& p_cpu, const DataRepresentation& p_gpu);

			void bind(char* p_destination);

			template <typename TType>
			Layout& operator=(const TType& p_data);

			Layout& operator[](const std::wstring& p_name);

		private:
			char* _destination;
			DataRepresentation _gpu;
			DataRepresentation _cpu;
			bool _tightlyPacked;

			std::unordered_map<std::wstring, Layout> _innerLayouts;

			void _pushData(const char* p_basePtr);
		};

		class Factory
		{
		friend class ShaderParser;

		private:
			std::string _typeName;
			BindingPoint _bindingPoint;
			Layout _layout;

		public:
			void setTypeName(const std::string& p_name);
			void setBindingPoint(BindingPoint p_bindingPoint);

			const std::string& typeName() const;
			BindingPoint bindingPoint() const;

			Layout& mainLayout();
			const Layout& mainLayout() const;
			Layout& addInnerLayout(Layout& p_layout, const std::wstring& p_name, const Layout::DataRepresentation& p_cpu, const Layout::DataRepresentation& p_gpu);
			UniformBufferObject construct() const;
		};

	private:
		std::string _typeName;
		BindingPoint _bindingPoint;
		BlockIndex _blockIndex;
		Layout _layout;

	public:
		UniformBufferObject();
		UniformBufferObject(UniformBufferObject&& p_other) noexcept;
		UniformBufferObject& operator=(UniformBufferObject&& p_other) noexcept;

		const std::string& typeName() const;
		const BindingPoint& bindingPoint() const;

		void activate();

		template <typename TType>
		UniformBufferObject& operator=(const TType& p_layout)
		{
			_layout = p_layout;
			validate();
			return *this;
		}

		Layout& operator[](const std::wstring& p_name);
	};
}
