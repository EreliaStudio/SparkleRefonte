#pragma once

#include "math/spk_vector3.hpp"
#include <cmath>
#include <iostream>

namespace spk
{
	class Quaternion
	{
	public:
		float x;
		float y;
		float z;
		float w;

		Quaternion();

		Quaternion(const float& p_x, const float& p_y, const float& p_z, const float& p_w);

		Quaternion(const Vector3& p_eulerAngles);

		Quaternion operator+(const Quaternion& rhs) const;

		Quaternion& operator+=(const Quaternion& rhs);

		Quaternion operator-(const Quaternion& rhs) const;

		Quaternion& operator-=(const Quaternion& rhs);

		Quaternion operator*(const float& scalar) const;

		Quaternion& operator*=(const float& scalar);

		Quaternion operator/(const float& scalar) const;

		Quaternion& operator/=(const float& scalar);

		friend Quaternion operator*(const float& scalar, const Quaternion& q)
		{
			return (q * scalar);
		}

		friend Quaternion operator/(const float& scalar, const Quaternion& q)
		{
			return (q / scalar);
		}

		bool operator==(const Quaternion& p_rhs) const;
		bool operator!=(const Quaternion& p_rhs) const;
		bool operator<(const Quaternion& p_rhs) const;
		bool operator>(const Quaternion& p_rhs) const;
		bool operator<=(const Quaternion& p_rhs) const;
		bool operator>=(const Quaternion& p_rhs) const;

		Quaternion normalize() const;

		Quaternion inverse() const;

		Quaternion conjugate() const;

		Quaternion operator*(const Quaternion& p_rhs) const;

		Quaternion& operator*=(const Quaternion& p_rhs);

		static Quaternion lookAt(const Vector3& p_direction, const Vector3& p_up);

		static Quaternion fromEulerAngles(const Vector3& p_eulerAngles);
		static Quaternion fromEulerAngles(const float& p_angleX, const float& p_angleY, const float& p_angleZ);

		static Quaternion fromAxisAngle(const Vector3& p_axis, float p_angleDegree);

		Vector3 applyRotation(const spk::Vector3& p_inputPoint) const;

		friend std::ostream& operator<<(std::ostream& p_os, const Quaternion& p_quaternion)
		{
			p_os << p_quaternion.x << " / " << p_quaternion.y << " / " << p_quaternion.z << " / " << p_quaternion.w;
			return (p_os);
		}

		Vector3 toVector3() const;
	};
}