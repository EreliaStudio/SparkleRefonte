#pragma once

#include "structure/graphics/spk_geometry_2D.hpp"

namespace spk
{
	class Viewport
	{
	private:
		Geometry2D _geometry;

	public:
		Viewport();
		Viewport(const Geometry2D& p_geometry);

		const Geometry2D& geometry() const;
		void setGeometry(const Geometry2D& p_geometry);

		void apply();
	};
}