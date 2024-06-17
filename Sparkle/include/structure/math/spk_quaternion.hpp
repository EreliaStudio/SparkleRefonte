#pragma once

#include <cmath>
#include <iostream>
#include <type_traits>
#include <array>
#include "structure/math/spk_math.hpp"
#include "structure/math/spk_vector2.hpp"
#include "structure/math/spk_vector3.hpp"
#include "structure/math/spk_matrix.hpp"

namespace spk
{
	template<typename TType>
	struct IQuaternion
	{
		static_assert(std::is_floating_point<TType>::value, "TType must be a floating point type.");

		TType w, x, y, z;

		IQuaternion() : w(1), x(0), y(0), z(0) {}

		template <typename TValueAType, typename TValueBType, typename TValueCType, typename TValueDType>
		IQuaternion(TValueAType wVal, TValueBType xVal, TValueCType yVal, TValueDType zVal) :
			w(static_cast<TType>(wVal)),
			x(static_cast<TType>(xVal)),
			y(static_cast<TType>(yVal)),
			z(static_cast<TType>(zVal))
		{
			static_assert(std::is_arithmetic<TValueAType>::value, "Input type must be a arithmetic type.");
			static_assert(std::is_arithmetic<TValueBType>::value, "Input type must be a arithmetic type.");
			static_assert(std::is_arithmetic<TValueCType>::value, "Input type must be a arithmetic type.");
			static_assert(std::is_arithmetic<TValueDType>::value, "Input type must be a arithmetic type.");
		}

		template <typename TOtherType>
		IQuaternion(const IQuaternion<TOtherType>& other) :
			w(static_cast<TType>(other.w)),
			x(static_cast<TType>(other.x)),
			y(static_cast<TType>(other.y)),
			z(static_cast<TType>(other.z))
		{
			static_assert(std::is_floating_point<TOtherType>::value, "Input type must be a arithmetic type.");
		}

		template <typename TOtherType>
		IQuaternion& operator=(const IQuaternion<TOtherType>& other)
		{
			static_assert(std::is_floating_point<TOtherType>::value, "Input type must be a arithmetic type.");

			w = other.w;
			x = other.x;
			y = other.y;
			z = other.z;
		}

		friend std::ostream& operator<<(std::ostream& os, const IQuaternion& quat)
		{
			os << "(" << quat.w << ", " << quat.x << ", " << quat.y << ", " << quat.z << ")";
			return os;
		}

		friend std::wostream& operator<<(std::wostream& os, const IQuaternion& quat)
		{
			os << "(" << quat.w << ", " << quat.x << ", " << quat.y << ", " << quat.z << ")";
			return (os);
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

		IQuaternion<TType> normalize() const
		{
			TType len = std::sqrt(w * w + x * x + y * y + z * z);
			if (len == 0)
				return IQuaternion<TType>(1, 0, 0, 0);
			return IQuaternion<TType>(w / len, x / len, y / len, z / len);
		}

		IQuaternion<TType> operator*(const IQuaternion<TType>& other) const
		{
			return IQuaternion<TType>(
				w * other.w - x * other.x - y * other.y - z * other.z,
				w * other.x + x * other.w + y * other.z - z * other.y,
				w * other.y - x * other.z + y * other.w + z * other.x,
				w * other.z + x * other.y - y * other.x + z * other.w);
		}

		template <typename TOtherType>
		IVector3<TOtherType> operator*(const IVector3<TOtherType>& other) const
		{
			IQuaternion<TType> qVec(0, other.x, other.y, other.z);
			IQuaternion<TType> qConj(w, -x, -y, -z);

			return (*this * qVec * qConj).normalize().toVector3();
		}

		template <typename TOtherType>
		bool operator==(const IQuaternion<TOtherType>& other) const
		{
			constexpr TType epsilon = static_cast<TType>(1e-5);

			return (std::fabs(w - static_cast<TType>(other.w)) < epsilon &&
				std::fabs(x - static_cast<TType>(other.x)) < epsilon &&
				std::fabs(y - static_cast<TType>(other.y)) < epsilon &&
				std::fabs(z - static_cast<TType>(other.z)) < epsilon
				);
		}

		static IQuaternion<TType> rotationFromEuler(TType roll, TType pitch, TType yaw)
		{
			TType halfPitch = spk::degreeToRadian(pitch) / 2;
			TType halfRoll = spk::degreeToRadian(roll) / 2;
			TType halfYaw = spk::degreeToRadian(yaw) / 2;

			TType sinPitch = std::sin(halfPitch);
			TType cosPitch = std::cos(halfPitch);
			TType sinRoll = std::sin(halfRoll);
			TType cosRoll = std::cos(halfRoll);
			TType sinYaw = std::sin(halfYaw);
			TType cosYaw = std::cos(halfYaw);

			return IQuaternion<TType>(
				cosYaw * cosPitch * cosRoll + sinYaw * sinPitch * sinRoll,
				cosYaw * cosPitch * sinRoll - sinYaw * sinPitch * cosRoll,
				cosYaw * sinPitch * cosRoll + sinYaw * cosPitch * sinRoll,
				sinYaw * cosPitch * cosRoll - cosYaw * sinPitch * sinRoll
			);
		}

		static IQuaternion<TType> rotateAroundAxis(const IVector3<TType>& axis, TType angle)
		{
			TType halfAngle = spk::degreeToRadian(angle) / 2;
			TType sinHalfAngle = std::sin(halfAngle);

			IVector3<TType> normAxis = axis.normalize();
			return IQuaternion<TType>(
				std::cos(halfAngle),
				normAxis.x * sinHalfAngle,
				normAxis.y * sinHalfAngle,
				normAxis.z * sinHalfAngle).normalize();
		}

		static IQuaternion<TType> lookAt(const IVector3<TType>& source, const IVector3<TType>& destination, const IVector3<TType>& up)
		{
			IVector3<TType> forward = (destination - source).normalize();
			IVector3<TType> right = up.cross(forward).normalize();
			IVector3<TType> newUp = forward.cross(right);

			TType trace = right.x + newUp.y + forward.z;
			if (trace > 0.0f)
			{
				TType s = 0.5f / std::sqrt(trace + 1.0f);
				return IQuaternion<TType>(
					0.25f / s,
					(newUp.z - forward.y) * s,
					(forward.x - right.z) * s,
					(right.y - newUp.x) * s);
			}
			else if ((right.x > newUp.y) && (right.x > forward.z))
			{
				TType s = 2.0f * std::sqrt(1.0f + right.x - newUp.y - forward.z);
				return IQuaternion<TType>(
					(newUp.z - forward.y) / s,
					0.25f * s,
					(newUp.x + right.y) / s,
					(forward.x + right.z) / s);
			}
			else if (newUp.y > forward.z)
			{
				TType s = 2.0f * std::sqrt(1.0f + newUp.y - right.x - forward.z);
				return IQuaternion<TType>(
					(forward.x - right.z) / s,
					(newUp.x + right.y) / s,
					0.25f * s,
					(forward.y + newUp.z) / s);
			}
			else
			{
				TType s = 2.0f * std::sqrt(1.0f + forward.z - right.x - newUp.y);
				return IQuaternion<TType>(
					(right.y - newUp.x) / s,
					(forward.x + right.z) / s,
					(forward.y + newUp.z) / s,
					0.25f * s);
			}
		}

		IMatrix<4, 4> toMatrix() const
		{
			IMatrix<4, 4> matrix;

			TType xx = x * x;
			TType yy = y * y;
			TType zz = z * z;
			TType xy = x * y;
			TType xz = x * z;
			TType yz = y * z;
			TType wx = w * x;
			TType wy = w * y;
			TType wz = w * z;

			matrix[0][0] = 1.0f - 2.0f * (yy + zz);
			matrix[1][0] = 2.0f * (xy + wz);
			matrix[2][0] = 2.0f * (xz - wy);
			matrix[3][0] = 0.0f;

			matrix[0][1] = 2.0f * (xy - wz);
			matrix[1][1] = 1.0f - 2.0f * (xx + zz);
			matrix[2][1] = 2.0f * (yz + wx);
			matrix[3][1] = 0.0f;

			matrix[0][2] = 2.0f * (xz + wy);
			matrix[1][2] = 2.0f * (yz - wx);
			matrix[2][2] = 1.0f - 2.0f * (xx + yy);
			matrix[3][2] = 0.0f;

			matrix[0][3] = 0.0f;
			matrix[1][3] = 0.0f;
			matrix[2][3] = 0.0f;
			matrix[3][3] = 1.0f;

			return matrix;
		}

		IVector3<TType> toVector3() const
		{
			return (IVector3<TType>(x, y, z));
		}
	};

	using Quaternion = IQuaternion<float_t>;
}