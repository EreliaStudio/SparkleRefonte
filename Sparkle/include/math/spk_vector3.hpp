#pragma once

#include "spk_define.hpp"

#include <iostream>
#include <type_traits>
#include <unordered_map>
#include <sstream>
#include <algorithm>

#include "math/spk_math.hpp"
#include "math/spk_vector2.hpp"

namespace spk
{
    template<typename TType>
    struct IVector3
    {
        static_assert(std::is_arithmetic<TType>::value, "TType must be an arithmetic type.");

        TType x, y, z;

        // Default constructor
        IVector3() : x(0), y(0), z(0) {}

        // Constructor with scalar
        template<typename UType, typename = std::enable_if_t<std::is_arithmetic<UType>::value>>
        IVector3(UType scalar) : x(static_cast<TType>(scalar)), y(static_cast<TType>(scalar)), z(static_cast<TType>(scalar)) {}

        // Constructor with three values
        template<typename UType, typename VType, typename WType,
            typename = std::enable_if_t<std::is_arithmetic<UType>::value&& std::is_arithmetic<VType>::value&& std::is_arithmetic<WType>::value>>
            IVector3(UType xVal, VType yVal, WType zVal) : x(static_cast<TType>(xVal)), y(static_cast<TType>(yVal)), z(static_cast<TType>(zVal)) {}

        // Copy constructor
        IVector3(const IVector3<TType>& other) = default;

        // Copy assignment operator
        IVector3& operator=(const IVector3<TType>& other) = default;

        // Stream insertion operator
        friend std::wostream& operator<<(std::wostream& os, const IVector3& vec)
        {
            os << L"(" << vec.x << L", " << vec.y << L", " << vec.z << L")";
            return os;
        }

        friend std::ostream& operator<<(std::ostream& os, const IVector3& vec)
        {
            os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
            return os;
        }

        // to_string method
        spk::String to_string() const
        {
            std::wstringstream wss;
            wss << *this;
            return wss.str();
        }

        // Equality operators
        template<typename UType>
        bool operator==(const IVector3<UType>& other) const
        {
            return x == static_cast<TType>(other.x) && y == static_cast<TType>(other.y) && z == static_cast<TType>(other.z);
        }

        template<typename UType>
        bool operator!=(const IVector3<UType>& other) const
        {
            return !(*this == other);
        }

        // Comparison operators
        template<typename UType>
        bool operator<(const IVector3<UType>& other) const
        {
            return x < other.x || (x == other.x && y < other.y) || (x == other.x && y == other.y && z < other.z);
        }

        template<typename UType>
        bool operator>(const IVector3<UType>& other) const
        {
            return x > other.x || (x == other.x && y > other.y) || (x == other.x && y == other.y && z > other.z);
        }

        template<typename UType>
        bool operator<=(const IVector3<UType>& other) const
        {
            return !(*this > other);
        }

        template<typename UType>
        bool operator>=(const IVector3<UType>& other) const
        {
            return !(*this < other);
        }

        // Comparison operators with scalar
        template<typename UType>
        bool operator==(UType scalar) const
        {
            return x == scalar && y == scalar && z == scalar;
        }

        template<typename UType>
        bool operator!=(UType scalar) const
        {
            return !(*this == scalar);
        }

        template<typename UType>
        bool operator<(UType scalar) const
        {
            return x < scalar && y < scalar && z < scalar;
        }

        template<typename UType>
        bool operator>(UType scalar) const
        {
            return x > scalar && y > scalar && z > scalar;
        }

        template<typename UType>
        bool operator<=(UType scalar) const
        {
            return x <= scalar && y <= scalar && z <= scalar;
        }

        template<typename UType>
        bool operator>=(UType scalar) const
        {
            return x >= scalar && y >= scalar && z >= scalar;
        }

        // Arithmetic operators with another vector
        template<typename UType>
        IVector3<TType> operator+(const IVector3<UType>& other) const
        {
            return IVector3<TType>(x + other.x, y + other.y, z + other.z);
        }

        template<typename UType>
        IVector3<TType> operator-(const IVector3<UType>& other) const
        {
            return IVector3<TType>(x - other.x, y - other.y, z - other.z);
        }

        template<typename UType>
        IVector3<TType> operator*(const IVector3<UType>& other) const
        {
            return IVector3<TType>(x * other.x, y * other.y, z * other.z);
        }

        template<typename UType>
        IVector3<TType> operator/(const IVector3<UType>& other) const
        {
            if (other.x == 0 || other.y == 0 || other.z == 0)
            {
                throw std::runtime_error("Division by zero");
            }
            return IVector3<TType>(x / other.x, y / other.y, z / other.z);
        }

        // Arithmetic operators with a scalar
        template<typename UType>
        IVector3<TType> operator+(UType scalar) const
        {
            return IVector3<TType>(x + scalar, y + scalar, z + scalar);
        }

        template<typename UType>
        IVector3<TType> operator-(UType scalar) const
        {
            return IVector3<TType>(x - scalar, y - scalar, z - scalar);
        }

        template<typename UType>
        IVector3<TType> operator*(UType scalar) const
        {
            return IVector3<TType>(x * scalar, y * scalar, z * scalar);
        }

        template<typename UType>
        IVector3<TType> operator/(UType scalar) const
        {
            if (scalar == 0)
            {
                throw std::runtime_error("Division by zero");
            }
            return IVector3<TType>(x / scalar, y / scalar, z / scalar);
        }

        // Compound assignment operators with another vector
        template<typename UType>
        IVector3<TType>& operator+=(const IVector3<UType>& other)
        {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        template<typename UType>
        IVector3<TType>& operator-=(const IVector3<UType>& other)
        {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }

        template<typename UType>
        IVector3<TType>& operator*=(const IVector3<UType>& other)
        {
            x *= other.x;
            y *= other.y;
            z *= other.z;
            return *this;
        }

        template<typename UType>
        IVector3<TType>& operator/=(const IVector3<UType>& other)
        {
            if (other.x == 0 || other.y == 0 || other.z == 0)
            {
                throw std::runtime_error("Division by zero");
            }
            x /= other.x;
            y /= other.y;
            z /= other.z;
            return *this;
        }

        // Compound assignment operators with a scalar
        template<typename UType>
        IVector3<TType>& operator+=(UType scalar)
        {
            x += static_cast<TType>(scalar);
            y += static_cast<TType>(scalar);
            z += static_cast<TType>(scalar);
            return *this;
        }

        template<typename UType>
        IVector3<TType>& operator-=(UType scalar)
        {
            x -= static_cast<TType>(scalar);
            y -= static_cast<TType>(scalar);
            z -= static_cast<TType>(scalar);
            return *this;
        }

        template<typename UType>
        IVector3<TType>& operator*=(UType scalar)
        {
            x *= static_cast<TType>(scalar);
            y *= static_cast<TType>(scalar);
            z *= static_cast<TType>(scalar);
            return *this;
        }

        template<typename UType>
        IVector3<TType>& operator/=(UType scalar)
        {
            if (scalar == 0)
            {
                throw std::runtime_error("Division by zero");
            }

            x /= static_cast<TType>(scalar);
            y /= static_cast<TType>(scalar);
            z /= static_cast<TType>(scalar);
            return *this;
        }

        float distance(const IVector3<TType>& p_other) const
        {
            return static_cast<float>(std::sqrt(std::pow(p_other.x - x, 2) + std::pow(p_other.y - y, 2) + std::pow(p_other.z - z, 2)));
        }

        float norm() const
        {
            return static_cast<float>(std::sqrt(x * x + y * y + z * z));
        }

        IVector3<float> normalize() const
        {
            float len = norm();
            if (len == 0) return (IVector3<float>(0, 0, 0));
            return IVector3<float>(x / len, y / len, z / len);
        }

        IVector3<TType> cross(const IVector3<TType>& p_other) const
        {
            return IVector3<TType>(
                y * p_other.z - z * p_other.y,
                z * p_other.x - x * p_other.z,
                x * p_other.y - y * p_other.x);
        }

        IVector3<TType> rotate(const IVector3<TType>& p_rotationValues) const
        {
            TType cos_x = std::cos(spk::degreeToRadian(static_cast<float>(p_rotationValues.x)));
            TType sin_x = std::sin(spk::degreeToRadian(static_cast<float>(p_rotationValues.x)));
            TType cos_y = std::cos(spk::degreeToRadian(static_cast<float>(p_rotationValues.y)));
            TType sin_y = std::sin(spk::degreeToRadian(static_cast<float>(p_rotationValues.y)));
            TType cos_z = std::cos(spk::degreeToRadian(static_cast<float>(p_rotationValues.z)));
            TType sin_z = std::sin(spk::degreeToRadian(static_cast<float>(p_rotationValues.z)));

            IVector3<TType> result;
            result.x = cos_y * cos_z * x + (sin_x * sin_y * cos_z - cos_x * sin_z) * y + (cos_x * sin_y * cos_z + sin_x * sin_z) * z;
            result.y = cos_y * sin_z * x + (sin_x * sin_y * sin_z + cos_x * cos_z) * y + (cos_x * sin_y * sin_z - sin_x * cos_z) * z;
            result.z = -sin_y * x + sin_x * cos_y * y + cos_x * cos_y * z;

            return result;
        }

        TType dot(const IVector3<TType>& p_other) const
        {
            return x * p_other.x + y * p_other.y + z * p_other.z;
        }

        IVector3 inverse() const
        {
            return IVector3(-x, -y, -z);
        }

        IVector2<TType> xy() const
        {
            return IVector2<TType>(x, y);
        }

        IVector2<TType> xz() const
        {
            return IVector2<TType>(x, z);
        }

        IVector2<TType> yz() const
        {
            return IVector2<TType>(y, z);
        }

        static IVector3 radianToDegree(const IVector3& radians)
        {
            return IVector3(
                spk::radianToDegree(radians.x),
                spk::radianToDegree(radians.y),
                spk::radianToDegree(radians.z)
            );
        }

        static IVector3 degreeToRadian(const IVector3& degrees)
        {
            return IVector3(
                spk::degreeToRadian(degrees.x),
                spk::degreeToRadian(degrees.y),
                spk::degreeToRadian(degrees.z)
            );
        }

        IVector3 clamp(const IVector3& p_lowerValue, const IVector3& p_higherValue)
        {
            return IVector3(
                std::clamp(x, p_lowerValue.x, p_higherValue.x),
                std::clamp(y, p_lowerValue.y, p_higherValue.y),
                std::clamp(z, p_lowerValue.z, p_higherValue.z)
            );
        }

        template <typename TOtherType>
        static IVector3 floor(const IVector3<TOtherType>& p_vector)
        {
            IVector3 result;
            result.x = std::floor(p_vector.x);
            result.y = std::floor(p_vector.y);
            result.z = std::floor(p_vector.z);
            return result;
        }

        template <typename TOtherType>
        static IVector3 ceiling(const IVector3<TOtherType>& p_vector)
        {
            IVector3 result;
            result.x = std::ceil(p_vector.x);
            result.y = std::ceil(p_vector.y);
            result.z = std::ceil(p_vector.z);
            return result;
        }

        template <typename TOtherType>
        static IVector3 round(const IVector3<TOtherType>& p_vector)
        {
            IVector3 result;
            result.x = std::round(p_vector.x);
            result.y = std::round(p_vector.y);
            result.z = std::round(p_vector.z);
            return result;
        }

        static IVector3 min(const IVector3& p_min, const IVector3& p_max)
        {
            return IVector3(std::min(p_min.x, p_max.x), std::min(p_min.y, p_max.y), std::min(p_min.z, p_max.z));
        }

        static IVector3 max(const IVector3& p_min, const IVector3& p_max)
        {
            return IVector3(std::max(p_min.x, p_max.x), std::max(p_min.y, p_max.y), std::max(p_min.z, p_max.z));
        }

        static IVector3 lerp(const IVector3& p_startingPoint, const IVector3& p_endingPoint, float t)
        {
            return IVector3(
                p_startingPoint.x + (p_endingPoint.x - p_startingPoint.x) * t,
                p_startingPoint.y + (p_endingPoint.y - p_startingPoint.y) * t,
                p_startingPoint.z + (p_endingPoint.z - p_startingPoint.z) * t
            );
        }
    };

    using Vector3 = IVector3<float_t>;
    using Vector3Int = IVector3<int32_t>;
    using Vector3UInt = IVector3<uint32_t>;
}

// Arithmetic operators with scalar outside of the class
template<typename TType, typename UType, typename = std::enable_if_t<std::is_arithmetic<UType>::value>>
spk::IVector3<TType> operator+(UType scalar, const spk::IVector3<TType>& vec)
{
    return spk::IVector3<TType>(scalar + vec.x, scalar + vec.y, scalar + vec.z);
}

template<typename TType, typename UType, typename = std::enable_if_t<std::is_arithmetic<UType>::value>>
spk::IVector3<TType> operator-(UType scalar, const spk::IVector3<TType>& vec)
{
    return spk::IVector3<TType>(scalar - vec.x, scalar - vec.y, scalar - vec.z);
}

template<typename TType, typename UType, typename = std::enable_if_t<std::is_arithmetic<UType>::value>>
spk::IVector3<TType> operator*(UType scalar, const spk::IVector3<TType>& vec)
{
    return spk::IVector3<TType>(scalar * vec.x, scalar * vec.y, scalar * vec.z);
}

template<typename TType, typename UType, typename = std::enable_if_t<std::is_arithmetic<UType>::value>>
spk::IVector3<TType> operator/(UType scalar, const spk::IVector3<TType>& vec)
{
    if (vec.x == 0 || vec.y == 0 || vec.z == 0)
    {
        throw std::runtime_error("Division by zero");
    }
    return spk::IVector3<TType>(scalar / vec.x, scalar / vec.y, scalar / vec.z);
}

// Hash function for IVector3 to use in unordered_map
namespace std
{
    template<typename TType>
    struct hash< spk::IVector3<TType> >
    {
        size_t operator()(const spk::IVector3<TType>& vec) const
        {
            return hash<TType>()(vec.x) ^ (hash<TType>()(vec.y) << 1) ^ (hash<TType>()(vec.z) << 2);
        }
    };
}
