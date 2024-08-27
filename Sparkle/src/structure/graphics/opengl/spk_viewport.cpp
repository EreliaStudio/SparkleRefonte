#include "structure/graphics/opengl/spk_viewport.hpp"

#include <GL/glew.h>

namespace spk
{
	Viewport::Viewport()
	{

	}

	Viewport::Viewport(const spk::Geometry2DInt& p_geometry) :
		_geometry(p_geometry)
	{

	}

	const spk::Geometry2DInt& Viewport::geometry() const
	{
		return (_geometry);
	}

	void Viewport::setGeometry(const spk::Geometry2DInt& p_geometry)
	{
		_geometry = p_geometry;
	}

	void Viewport::apply()
	{
		glViewport(_geometry.x, _geometry.y, _geometry.width, _geometry.height);
	}
}