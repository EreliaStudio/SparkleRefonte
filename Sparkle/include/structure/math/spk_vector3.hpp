#pragma once

#include "spk_define.hpp"

#include <iostream>
#include <type_traits>
#include <unordered_map>
#include <sstream>

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
    };
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
