#pragma once

#include "structure/graphics/spk_geometry_2D.hpp"

namespace spk
{
	class Viewport
	{
	private:
		spk::Geometry2DInt _geometry;

	public:
		Viewport();
		Viewport(const spk::Geometry2DInt& p_geometry);

		const spk::Geometry2DInt& geometry() const;
		void setGeometry(const spk::Geometry2DInt& p_geometry);

		void apply();
	};
}