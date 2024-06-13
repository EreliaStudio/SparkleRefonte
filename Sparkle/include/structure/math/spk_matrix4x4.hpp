#pragma once

#include <iomanip>
#include <cmath>

#include "structure/math/spk_vector2.hpp"
#include "structure/math/spk_vector3.hpp"

namespace spk
{
	struct Matrix4x4
	{
		float data[4][4] = {
			{1, 0, 0, 0},
			{0, 1, 0, 0},
			{0, 0, 1, 0},
			{0, 0, 0, 1} };

		Matrix4x4();

		Matrix4x4(float a1, float a2, float a3, float a4,  // First row
			float b1, float b2, float b3, float b4,  // Second row
			float c1, float c2, float c3, float c4,  // Third row
			float d1, float d2, float d3, float d4); // Forth row

		Matrix4x4 operator+(const Matrix4x4& other) const;

		Vector3 operator*(const Vector3& v) const;

		Vector2 operator*(const Vector2& v) const;

		Matrix4x4 operator*(const Matrix4x4& other) const;

		static Matrix4x4 lookAt(const Vector3& p_from, const Vector3& p_to, const Vector3& p_up = Vector3(0, 1, 0));

		static Matrix4x4 translationMatrix(const Vector3& p_translation);

		static Matrix4x4 scaleMatrix(const Vector3& p_scale);

		static Matrix4x4 rotationMatrix(const Vector3& p_rotation);

		static Matrix4x4 rotateAroundAxis(const Vector3& p_axis, const float& p_rotationAngle);

		static Matrix4x4 perspective(float fov, float aspectRatio, float nearPlane, float farPlane);

		static Matrix4x4 ortho(float left, float right, float bottom, float top, float nearPlane, float farPlane);

		bool operator==(const Matrix4x4& other) const;

		bool operator!=(const Matrix4x4& other) const;

		Matrix4x4 transpose() const;

		float determinant2x2(float a, float b, float c, float d);

		Matrix4x4 inverse() const;

		friend std::wostream& operator<<(std::wostream& p_os, const Matrix4x4& p_matrix)
		{
			p_os << std::endl;
			for (size_t y = 0; y < 4; y++)
			{
				for (size_t x = 0; x < 4; x++)
				{
					if (x != 0)
						p_os << " ";
					p_os << p_matrix.data[x][y];
				}
				p_os << std::endl;
			}

			return p_os;
		}

		float* ptr();

		const float* c_ptr() const;
	};
}