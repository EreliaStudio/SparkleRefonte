#pragma once

#include <iostream>
#include "structure/math/spk_vector2.hpp"

namespace spk
{
	struct Geometry2D
	{
		using Point = IVector2<int>;
		using Size = IVector2<size_t>;

		union
		{
			Point anchor;
			struct
			{
				int x;
				int y;
			};
		};

		union
		{
			Size size;
			struct
			{
				size_t width;
				size_t heigth;
			};
		};

		Geometry2D() :
			anchor(0, 0),
			size(0, 0)
		{

		}

		Geometry2D(const Point& p_anchor, const Size& p_size) :
			anchor(p_anchor),
			size(p_size)
		{

		}

		Geometry2D(const Point& p_anchor, size_t p_width, size_t p_heigth) :
			anchor(p_anchor),
			size(p_width, p_heigth)
		{

		}

		Geometry2D(int p_x, int p_y, const Size& p_size) :
			anchor(p_x, p_y),
			size(p_size)
		{

		}

		Geometry2D(int p_x, int p_y, size_t p_width, size_t p_heigth) :
			anchor(p_x, p_y),
			size(p_width, p_heigth)
		{

		}

		friend std::wostream& operator<<(std::wostream& os, const Geometry2D& p_geometry)
		{
			os << L"Anchor : " << p_geometry.anchor << L" - Size : " << p_geometry.size;
			return os;
		}

		bool operator==(const Geometry2D& p_other) const
		{
			return anchor == p_other.anchor && size == p_other.size;
		}

		bool operator!=(const Geometry2D& p_other) const
		{
			return !(*this == p_other);
		}
	};
}
