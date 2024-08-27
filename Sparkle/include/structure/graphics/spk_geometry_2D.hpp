#pragma once

#include <iostream>

namespace spk
{
	template <typename TType>
	struct IGeometry2D
	{
		struct Position
		{
			TType x;
			TType y;

			Position() :
				x(0),
				y(0)
			{

			}

			template<typename TScalarType>
			explicit Position(const TScalarType& p_scalar) :
				x(static_cast<TType>(p_scalar)),
				y(static_cast<TType>(p_scalar))
			{

			}

			template<typename TValueAType, typename TValueBType>
			Position(const TValueAType& p_valueA, const TValueBType& p_valueB) :
				x(static_cast<TType>(p_valueA)),
				y(static_cast<TType>(p_valueB))
			{

			}

			template<typename TScalarType>
			Position operator+(const TScalarType& p_scalar) const
			{
				return { x + p_scalar, y + p_scalar };
			}

			Position operator+(Position p_other) const
			{
				return { x + p_other.x, y + p_other.y };
			}

			template<typename TScalarType>
			Position operator-(const TScalarType& p_scalar) const
			{
				return { x - p_scalar, y - p_scalar };
			}

			Position operator-(Position p_other) const
			{
				return { x - p_other.x, y - p_other.y };
			}

			template<typename TScalarType>
			Position operator*(const TScalarType& p_scalar) const
			{
				return { x * p_scalar, y * p_scalar };
			}

			Position operator*(Position p_other) const
			{
				return { x * p_other.x, y * p_other.y };
			}

			template<typename TScalarType>
			Position operator/(const TScalarType& p_scalar) const
			{
				if (p_scalar == 0)
					throw std::runtime_error("Can't divide by zero");
				return { x / p_scalar, y / p_scalar };
			}

			Position operator/(Position p_other) const
			{
				if (p_other.x == 0 || p_other.y == 0)
					throw std::runtime_error("Can't divide by zero");
				return { x / p_other.x, y / p_other.y };
			}

			template<typename TScalarType>
			Position& operator+=(const TScalarType& p_scalar)
			{
				x += p_scalar;
				y += p_scalar;
				return *this;
			}

			Position& operator+=(Position p_other)
			{
				x += p_other.x;
				y += p_other.y;
				return *this;
			}

			template<typename TScalarType>
			Position& operator-=(const TScalarType& p_scalar)
			{
				x -= p_scalar;
				y -= p_scalar;
				return *this;
			}

			Position& operator-=(Position p_other)
			{
				x -= p_other.x;
				y -= p_other.y;
				return *this;
			}

			template<typename TScalarType>
			Position& operator*=(const TScalarType& p_scalar)
			{
				x *= p_scalar;
				y *= p_scalar;
				return *this;
			}

			Position& operator*=(Position p_other)
			{
				x *= p_other.x;
				y *= p_other.y;
				return *this;
			}

			template<typename TScalarType>
			Position& operator/=(const TScalarType& p_scalar)
			{
				if (p_scalar == 0)
					throw std::runtime_error("Can't divide by zero");
				x /= p_scalar;
				y /= p_scalar;
				return *this;
			}

			Position& operator/=(Position p_other)
			{
				if (p_other.x == 0 || p_other.y == 0)
					throw std::runtime_error("Can't divide by zero");
				x /= p_other.x;
				y /= p_other.y;
				return *this;
			}

			bool operator==(const Position& p_other) const
			{
				return x == p_other.x && y == p_other.y;
			}

			bool operator!=(const Position& p_other) const
			{
				return !(*this == p_other);
			}

			template<typename TScalarType>
			bool operator==(const TScalarType& p_scalar) const
			{
				return x == p_scalar && y == p_scalar;
			}

			template<typename TScalarType>
			bool operator!=(const TScalarType& p_scalar) const
			{
				return !(*this == p_scalar);
			}

			friend std::ostream& operator<<(std::ostream& p_os, const Position& p_position)
			{
				p_os << "(" << p_position.x << " x " << p_position.y << ")";
				return p_os;
			}

			friend std::wostream& operator<<(std::wostream& p_os, const Position& p_position)
			{
				p_os << L"(" << p_position.x << L" x " << p_position.y << L")";
				return p_os;
			}

			static Position min(const Position& p_min, const Position& p_max)
			{
				return Position(std::min(p_min.x, p_max.x), std::min(p_min.y, p_max.y));
			}

			static Position max(const Position& p_min, const Position& p_max)
			{
				return Position(std::max(p_min.x, p_max.x), std::max(p_min.y, p_max.y));
			}

			std::wstring to_wstring() const
			{
				std::wstringstream wss;
				wss << *this;
				return wss.str();
			}

			std::string to_string() const
			{
				std::stringstream ss;
				ss << *this;
				return ss.str();
			}
		};

		struct Size
		{
			TType width;
			TType heigth;

			Size() :
				width(0),
				heigth(0)
			{

			}

			template<typename TScalarType>
			explicit Size(const TScalarType& p_scalar) :
				width(static_cast<TType>(p_scalar)),
				heigth(static_cast<TType>(p_scalar))
			{

			}

			template<typename TValueAType, typename TValueBType>
			Size(const TValueAType& p_valueA, const TValueBType& p_valueB) :
				width(static_cast<TType>(p_valueA)),
				heigth(static_cast<TType>(p_valueB))
			{

			}

			template<typename TScalarType>
			Size operator+(const TScalarType& p_scalar) const
			{
				return { width + p_scalar, heigth + p_scalar };
			}

			Size operator+(const Size& p_other) const
			{
				return { width + p_other.width, heigth + p_other.heigth };
			}

			template<typename TScalarType>
			Size operator-(const TScalarType& p_scalar) const
			{
				return { width - p_scalar, heigth - p_scalar };
			}

			Size operator-(const Size& p_other) const
			{
				return { width - p_other.width, heigth - p_other.heigth };
			}

			template<typename TScalarType>
			Size operator*(const TScalarType& p_scalar) const
			{
				return { width * p_scalar, heigth * p_scalar };
			}

			Size operator*(const Size& p_other) const
			{
				return { width * p_other.width, heigth * p_other.heigth };
			}

			template<typename TScalarType>
			Size operator/(const TScalarType& p_scalar) const
			{
				if (p_scalar == 0)
					throw std::runtime_error("Can't divide by zero");
				return { width / p_scalar, heigth / p_scalar };
			}

			Size operator/(const Size& p_other) const
			{
				if (p_other.width == 0 || p_other.heigth == 0)
					throw std::runtime_error("Can't divide by zero");
				return { width / p_other.width, heigth / p_other.heigth };
			}

			template<typename TScalarType>
			Size& operator+=(const TScalarType& p_scalar)
			{
				width += p_scalar;
				heigth += p_scalar;
				return *this;
			}

			Size& operator+=(const Size& p_other)
			{
				width += p_other.width;
				heigth += p_other.heigth;
				return *this;
			}

			template<typename TScalarType>
			Size& operator-=(const TScalarType& p_scalar)
			{
				width -= p_scalar;
				heigth -= p_scalar;
				return *this;
			}

			Size& operator-=(const Size& p_other)
			{
				width -= p_other.width;
				heigth -= p_other.heigth;
				return *this;
			}

			template<typename TScalarType>
			Size& operator*=(const TScalarType& p_scalar)
			{
				width *= p_scalar;
				heigth *= p_scalar;
				return *this;
			}

			Size& operator*=(const Size& p_other)
			{
				width *= p_other.width;
				heigth *= p_other.heigth;
				return *this;
			}

			template<typename TScalarType>
			Size& operator/=(const TScalarType& p_scalar)
			{
				if (p_scalar == 0)
					throw std::runtime_error("Can't divide by zero");
				width /= p_scalar;
				heigth /= p_scalar;
				return *this;
			}

			Size& operator/=(const Size& p_other)
			{
				if (p_other.width == 0 || p_other.heigth == 0)
					throw std::runtime_error("Can't divide by zero");
				width /= p_other.width;
				heigth /= p_other.heigth;
				return *this;
			}

			bool operator==(const Size& p_other) const
			{
				return width == p_other.width && heigth == p_other.heigth;
			}

			bool operator!=(const Size& p_other) const
			{
				return !(*this == p_other);
			}

			template<typename TScalarType>
			bool operator==(const TScalarType& p_scalar) const
			{
				return width == p_scalar && heigth == p_scalar;
			}

			template<typename TScalarType>
			bool operator!=(const TScalarType& p_scalar) const
			{
				return !(*this == p_scalar);
			}

			friend std::wostream& operator<<(std::wostream& p_os, const Size& p_size)
			{
				p_os << L"(" << p_size.width << L" x " << p_size.heigth << L")";
				return p_os;
			}

			static Size min(const Size& p_min, const Size& p_max)
			{
				return Size(std::min(p_min.width, p_max.width), std::min(p_min.heigth, p_max.heigth));
			}

			static Size max(const Size& p_min, const Size& p_max)
			{
				return Size(std::max(p_min.width, p_max.width), std::max(p_min.heigth, p_max.heigth));
			}

			std::wstring to_wstring() const
			{
				std::wstringstream wss;
				wss << *this;
				return wss.str();
			}

			std::string to_string() const
			{
				std::stringstream ss;
				ss << *this;
				return ss.str();
			}
		};

		union
		{
			Position anchor;
			struct
			{
				TType x;
				TType y;
			};
		};

		union
		{
			Size size;
			struct
			{
				TType width;
				TType heigth;
			};
		};

		IGeometry2D() :
			anchor(0, 0),
			size(0, 0)
		{

		}

		IGeometry2D(const Position& p_anchor, const Size& p_size) :
			anchor(p_anchor),
			size(p_size)
		{

		}

		template<typename TValueAType, typename TValueBType>
		IGeometry2D(const Position& p_anchor, TValueAType p_width, TValueBType p_heigth) :
			anchor(p_anchor),
			size(static_cast<TType>(p_width), static_cast<TType>(p_heigth))
		{

		}

		template<typename TValueAType, typename TValueBType>
		IGeometry2D(TValueAType p_x, TValueBType p_y, const Size& p_size) :
			anchor(static_cast<TType>(p_x), static_cast<TType>(p_y)),
			size(p_size)
		{

		}

		template<typename TValueAType, typename TValueBType, typename TValueCType, typename TValueDType>
		IGeometry2D(TValueAType p_x, TValueBType p_y, TValueCType p_width, TValueDType p_heigth) :
			anchor(static_cast<TType>(p_x), static_cast<TType>(p_y)),
			size(static_cast<TType>(p_width), static_cast<TType>(p_heigth))
		{

		}

		friend std::wostream& operator<<(std::wostream& os, const IGeometry2D& p_geometry)
		{
			os << L"Anchor : " << p_geometry.anchor << L" - Size : " << p_geometry.size;
			return os;
		}

		bool operator==(const IGeometry2D& p_other) const
		{
			return anchor == p_other.anchor && size == p_other.size;
		}

		bool operator!=(const IGeometry2D& p_other) const
		{
			return !(*this == p_other);
		}
	};

	using Geometry2D = IGeometry2D<float>;
	using Geometry2DInt = IGeometry2D<int>;
}
