#pragma once

#include "spk_define.hpp"

#include <iostream>
#include <type_traits>
#include <unordered_map>
#include <sstream>

namespace spk
{
    template<typename TType>
    struct IVector2
    {
        static_assert(std::is_arithmetic<TType>::value, "TType must be an arithmetic type.");

        TType x, y;

        // Default constructor
        IVector2() : x(0), y(0) {}

        // Constructor with scalar
        template<typename UType, typename = std::enable_if_t<std::is_arithmetic<UType>::value>>
        IVector2(UType scalar) : x(static_cast<TType>(scalar)), y(static_cast<TType>(scalar)) {}

        // Constructor with two values
        template<typename UType, typename VType,
            typename = std::enable_if_t<std::is_arithmetic<UType>::value&& std::is_arithmetic<VType>::value>>
            IVector2(UType xVal, VType yVal) : x(static_cast<TType>(xVal)), y(static_cast<TType>(yVal)) {}

        // Copy constructor
        IVector2(const IVector2<TType>& other) = default;

        // Copy assignment operator
        IVector2& operator=(const IVector2<TType>& other) = default;

        // Stream insertion operator
        friend std::wostream& operator<<(std::wostream& os, const IVector2& vec)
        {
            os << L"(" << vec.x << L", " << vec.y << L")";
            return os;
        }

        friend std::ostream& operator<<(std::ostream& os, const IVector2& vec)
        {
            os << "(" << vec.x << ", " << vec.y << ")";
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
        bool operator==(const IVector2<UType>& other) const
        {
            return x == static_cast<TType>(other.x) && y == static_cast<TType>(other.y);
        }

        template<typename UType>
        bool operator!=(const IVector2<UType>& other) const
        {
            return !(*this == other);
        }

        // Comparison operators
        template<typename UType>
        bool operator<(const IVector2<UType>& other) const
        {
            return x < other.x || (x == other.x && y < other.y);
        }

        template<typename UType>
        bool operator>(const IVector2<UType>& other) const
        {
            return x > other.x || (x == other.x && y > other.y);
        }

        template<typename UType>
        bool operator<=(const IVector2<UType>& other) const
        {
            return !(*this > other);
        }

        template<typename UType>
        bool operator>=(const IVector2<UType>& other) const
        {
            return !(*this < other);
        }

        // Comparison operators with scalar
        template<typename UType>
        bool operator==(UType scalar) const
        {
            return x == scalar && y == scalar;
        }

        template<typename UType>
        bool operator!=(UType scalar) const
        {
            return !(*this == scalar);
        }

        template<typename UType>
        bool operator<(UType scalar) const
        {
            return x < scalar && y < scalar;
        }

        template<typename UType>
        bool operator>(UType scalar) const
        {
            return x > scalar && y > scalar;
        }

        template<typename UType>
        bool operator<=(UType scalar) const
        {
            return x <= scalar && y <= scalar;
        }

        template<typename UType>
        bool operator>=(UType scalar) const
        {
            return x >= scalar && y >= scalar;
        }

        // Arithmetic operators with another vector
        template<typename UType>
        IVector2<TType> operator+(const IVector2<UType>& other) const
        {
            return IVector2<TType>(x + other.x, y + other.y);
        }

        template<typename UType>
        IVector2<TType> operator-(const IVector2<UType>& other) const
        {
            return IVector2<TType>(x - other.x, y - other.y);
        }

        template<typename UType>
        IVector2<TType> operator*(const IVector2<UType>& other) const
        {
            return IVector2<TType>(x * other.x, y * other.y);
        }

        template<typename UType>
        IVector2<TType> operator/(const IVector2<UType>& other) const
        {
            return IVector2<TType>(x / other.x, y / other.y);
        }

        // Arithmetic operators with a scalar
        template<typename UType>
        IVector2<TType> operator+(UType scalar) const
        {
            return IVector2<TType>(x + scalar, y + scalar);
        }

        template<typename UType>
        IVector2<TType> operator-(UType scalar) const
        {
            return IVector2<TType>(x - scalar, y - scalar);
        }

        template<typename UType>
        IVector2<TType> operator*(UType scalar) const
        {
            return IVector2<TType>(x * scalar, y * scalar);
        }

        template<typename UType>
        IVector2<TType> operator/(UType scalar) const
        {
            return IVector2<TType>(x / scalar, y / scalar);
        }

        // Compound assignment operators with another vector
        template<typename UType>
        IVector2<TType>& operator+=(const IVector2<UType>& other)
        {
            x += other.x;
            y += other.y;
            return *this;
        }

        template<typename UType>
        IVector2<TType>& operator-=(const IVector2<UType>& other)
        {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        template<typename UType>
        IVector2<TType>& operator*=(const IVector2<UType>& other)
        {
            x *= other.x;
            y *= other.y;
            return *this;
        }

        template<typename UType>
        IVector2<TType>& operator/=(const IVector2<UType>& other)
        {
            x /= other.x;
            y /= other.y;
            return *this;
        }

        // Compound assignment operators with a scalar
        template<typename UType>
        IVector2<TType>& operator+=(UType scalar)
        {
            x += static_cast<TType>(scalar);
            y += static_cast<TType>(scalar);
            return *this;
        }

        template<typename UType>
        IVector2<TType>& operator-=(UType scalar)
        {
            x -= static_cast<TType>(scalar);
            y -= static_cast<TType>(scalar);
            return *this;
        }

        template<typename UType>
        IVector2<TType>& operator*=(UType scalar)
        {
            x *= static_cast<TType>(scalar);
            y *= static_cast<TType>(scalar);
            return *this;
        }

        template<typename UType>
        IVector2<TType>& operator/=(UType scalar)
        {
            x /= static_cast<TType>(scalar);
            y /= static_cast<TType>(scalar);
            return *this;
        }
    };
}

// Arithmetic operators with scalar outside of the class
template<typename TType, typename UType, typename = std::enable_if_t<std::is_arithmetic<UType>::value>>
spk::IVector2<TType> operator+(UType scalar, const spk::IVector2<TType>& vec)
{
    return spk::IVector2<TType>(scalar + vec.x, scalar + vec.y);
}

template<typename TType, typename UType, typename = std::enable_if_t<std::is_arithmetic<UType>::value>>
spk::IVector2<TType> operator-(UType scalar, const spk::IVector2<TType>& vec)
{
    return spk::IVector2<TType>(scalar - vec.x, scalar - vec.y);
}

template<typename TType, typename UType, typename = std::enable_if_t<std::is_arithmetic<UType>::value>>
spk::IVector2<TType> operator*(UType scalar, const spk::IVector2<TType>& vec)
{
    return spk::IVector2<TType>(scalar * vec.x, scalar * vec.y);
}

template<typename TType, typename UType, typename = std::enable_if_t<std::is_arithmetic<UType>::value>>
spk::IVector2<TType> operator/(UType scalar, const spk::IVector2<TType>& vec)
{
    return spk::IVector2<TType>(scalar / vec.x, scalar / vec.y);
}

// Hash function for IVector2 to use in unordered_map
namespace std
{
    template<typename TType>
    struct hash< spk::IVector2<TType> >
    {
        size_t operator()(const spk::IVector2<TType>& vec) const
        {
            return hash<TType>()(vec.x) ^ (hash<TType>()(vec.y) << 1);
        }
    };
}
