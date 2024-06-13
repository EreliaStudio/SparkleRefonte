#pragma once

#include <stdexcept>
#include <string>

#include "math/spk_vector2.hpp"
#include "math/spk_vector3.hpp"

namespace spk
{
	template <size_t SizeX, size_t SizeY>
	class IMatrix
	{
	public:
		class Column
		{
		private:
			float rows[SizeY];

		public:
			float& operator[](size_t p_index)
			{
				if (p_index >= SizeY)
					throw std::invalid_argument("Can't access the row " + std::to_string(p_index) + " on a matrix " + std::to_string(SizeX) + "x" + std::to_string(SizeY));
				return (rows[p_index]);
			}
			const float& operator[](size_t p_index) const
			{
				if (p_index >= SizeY)
					throw std::invalid_argument("Can't access the row " + std::to_string(p_index) + " on a matrix " + std::to_string(SizeX) + "x" + std::to_string(SizeY));
				return (rows[p_index]);
			}
		};

	private:
		Column cols[SizeX];

	public:
		IMatrix()
		{
			for (size_t i = 0; i < SizeX; i++)
			{
				for (size_t j = 0; j < SizeY; j++)
				{
					(*this)[i][j] = (i == j ? 1.0f : 0.0f);
				}
			}
		}

		IMatrix(float* p_values)
		{
			for (size_t i = 0; i < SizeX; i++)
			{
				for (size_t j = 0; j < SizeY; j++)
				{
					(*this)[i][j] = p_values[i * SizeY + j];
				}
			}
		}

		IMatrix(std::initializer_list<float> values)
		{
			if (values.size() != SizeX * SizeY)
			{
				throw std::invalid_argument("Initializer list size does not match the matrix dimensions.");
			}

			auto it = values.begin();
			for (size_t j = 0; j < SizeY; j++)
			{
				for (size_t i = 0; i < SizeX; i++)
				{
					(*this)[i][j] = *it++;
				}
			}
		}

		Column& operator[](size_t p_index)
		{
			if (p_index >= SizeX)
				throw std::invalid_argument("Can't access the column " + std::to_string(p_index) + " on a matrix " + std::to_string(SizeX) + "x" + std::to_string(SizeY));
			return (cols[p_index]);
		}
		const Column& operator[](size_t p_index) const
		{
			if (p_index >= SizeX)
				throw std::invalid_argument("Can't access the column " + std::to_string(p_index) + " on a matrix " + std::to_string(SizeX) + "x" + std::to_string(SizeY));
			return (cols[p_index]);
		}

		Vector3 operator*(const Vector3& vec) const
		{
			if (SizeX != 4 || SizeY != 4)
			{
				throw std::invalid_argument("Matrix must be 4x4 for this operation.");
			}

			Vector3 result;
			result.x = (*this)[0][0] * vec.x + (*this)[1][0] * vec.y + (*this)[2][0] * vec.z + (*this)[3][0] * 1;
			result.y = (*this)[0][1] * vec.x + (*this)[1][1] * vec.y + (*this)[2][1] * vec.z + (*this)[3][1] * 1;
			result.z = (*this)[0][2] * vec.x + (*this)[1][2] * vec.y + (*this)[2][2] * vec.z + (*this)[3][2] * 1;

			return result;
		}

		Vector2 operator*(const Vector2& vec) const
		{
			if (SizeX != 3 || SizeY != 3)
			{
				throw std::invalid_argument("Matrix must be 3x3 for this operation.");
			}

			Vector2 result;
			result.x = (*this)[0][0] * vec.x + (*this)[1][0] * vec.y + (*this)[2][0] * 1;
			result.y = (*this)[0][1] * vec.x + (*this)[1][1] * vec.y + (*this)[2][1] * 1;

			return result;
		}

		IMatrix<SizeX, SizeY> operator*(const IMatrix<SizeX, SizeY>& other) const
		{
			IMatrix<SizeX, SizeY> result;

			for (size_t i = 0; i < SizeX; ++i)
			{
				for (size_t j = 0; j < SizeY; ++j)
				{
					result[i][j] = 0;
					for (size_t k = 0; k < SizeX; ++k)
					{
						result[i][j] += (*this)[i][k] * other[k][j];
					}
				}
			}

			return result;
		}

		bool operator==(const IMatrix& other) const
		{
			for (size_t i = 0; i < SizeX; ++i)
			{
				for (size_t j = 0; j < SizeY; ++j)
				{
					if ((*this)[i][j] != other[i][j])
					{
						return false;
					}
				}
			}
			return true;
		}

		bool operator!=(const IMatrix& other) const
		{
			return !(*this == other);
		}

		static IMatrix<4, 4> rotationMatrix(float angleX, float angleY, float angleZ)
		{
			IMatrix<4, 4> mat;

			float cosX = std::cos(spk::degreeToRadian(angleX));
			float sinX = std::sin(spk::degreeToRadian(angleX));

			float cosY = std::cos(spk::degreeToRadian(angleY));
			float sinY = std::sin(spk::degreeToRadian(angleY));

			float cosZ = std::cos(spk::degreeToRadian(angleZ));
			float sinZ = std::sin(spk::degreeToRadian(angleZ));

			IMatrix<4, 4> rotationX = {
				1, 0, 0, 0,
				0, cosX, -sinX, 0,
				0, sinX, cosX, 0,
				0, 0, 0, 1
			};

			IMatrix<4, 4> rotationY = {
				cosY, 0, sinY, 0,
				0, 1, 0, 0,
				-sinY, 0, cosY, 0,
				0, 0, 0, 1
			};

			IMatrix<4, 4> rotationZ = {
				cosZ, -sinZ, 0, 0,
				sinZ, cosZ, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1
			};

			IMatrix<4, 4> rotation = rotationZ * rotationY * rotationX;

			return rotation;
		}

		static IMatrix<4, 4> rotationMatrix(spk::Vector3 p_angle)
		{
			return (rotationMatrix(p_angle.x, p_angle.y, p_angle.z));
		}

		static IMatrix<4, 4> translationMatrix(float translateX, float translateY, float translateZ)
		{
			IMatrix<4, 4> mat = {
				1, 0, 0, translateX,
				0, 1, 0, translateY,
				0, 0, 1, translateZ,
				0, 0, 0, 1
			};

			return mat;
		}

		static IMatrix<4, 4> translationMatrix(spk::Vector3 p_translation)
		{
			return (translationMatrix(p_translation.x, p_translation.y, p_translation.z));
		}

		// Scale Matrix
		static IMatrix<4, 4> scaleMatrix(float scaleX, float scaleY, float scaleZ)
		{
			IMatrix<4, 4> mat = {
				scaleX, 0, 0, 0,
				0, scaleY, 0, 0,
				0, 0, scaleZ, 0,
				0, 0, 0, 1
			};

			return mat;
		}

		static IMatrix<4, 4> scaleMatrix(spk::Vector3 p_scale)
		{
			return (scaleMatrix(p_scale.x, p_scale.y, p_scale.z));
		}

		spk::String to_string() const
		{
			std::wstringstream wss;
			for (size_t j = 0; j < SizeY; ++j)
			{
				if (j != 0)
					wss << " - ";

				wss << L"[";
				for (size_t i = 0; i < SizeX; ++i)
				{
					if (i != 0)
						wss << L" - ";
					wss << (*this)[i][j];
				}
				wss << L"]";
			}
			return wss.str();
		}

		friend std::wostream& operator<<(std::wostream& os, const IMatrix& matrix)
		{
			os << matrix.to_string();
			return os;
		}

		friend std::ostream& operator<<(std::ostream& os, const IMatrix& matrix)
		{
			std::wstring wstr = matrix.to_string();
			os << std::string(wstr.begin(), wstr.end());
			return os;
		}
	};

	using Matrix3x3 = IMatrix<3, 3>;
	using Matrix4x4 = IMatrix<4, 4>;
}