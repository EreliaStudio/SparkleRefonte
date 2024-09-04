#include "structure/graphics/opengl/spk_viewport.hpp"

#include <GL/glew.h>

#include "structure/spk_iostream.hpp"

namespace spk
{
	Viewport::Viewport()
	{

	}

	Viewport::Viewport(const Geometry2D& p_geometry) :
		_geometry(p_geometry)
	{

	}

	const Geometry2D& Viewport::geometry() const
	{
		return (_geometry);
	}

	void Viewport::setGeometry(const Geometry2D& p_geometry)
	{
		_geometry = p_geometry;
	}

	void Viewport::apply()
	{
		glViewport(static_cast<GLint>(_geometry.x), static_cast<GLint>(_geometry.y), static_cast<GLsizei>(_geometry.width), static_cast<GLsizei>(_geometry.heigth));
	}
}