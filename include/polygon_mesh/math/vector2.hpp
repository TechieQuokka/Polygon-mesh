#pragma once

#include <cmath>
#include <ostream>
#include <type_traits>
#include <stdexcept>

namespace polygon_mesh {
namespace math {

template<typename T>
class Vector2 {
    static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");

public:
    T x, y;

    // Constructors
    Vector2() : x(0), y(0) {}
    Vector2(T x, T y) : x(x), y(y) {}
    Vector2(T value) : x(value), y(value) {}

    // Copy and assignment
    Vector2(const Vector2& other) = default;
    Vector2& operator=(const Vector2& other) = default;
    Vector2(Vector2&& other) noexcept = default;
    Vector2& operator=(Vector2&& other) noexcept = default;

    // Arithmetic operators
    Vector2 operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }

    Vector2 operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }

    Vector2 operator*(T scalar) const {
        return Vector2(x * scalar, y * scalar);
    }

    Vector2 operator/(T scalar) const {
        return Vector2(x / scalar, y / scalar);
    }

    Vector2 operator-() const {
        return Vector2(-x, -y);
    }

    // Compound assignment operators
    Vector2& operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vector2& operator-=(const Vector2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vector2& operator*=(T scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Vector2& operator/=(T scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    // Comparison operators
    bool operator==(const Vector2& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Vector2& other) const {
        return !(*this == other);
    }

    // Element access
    T& operator[](std::size_t index) {
        switch (index) {
            case 0: return x;
            case 1: return y;
            default: throw std::out_of_range("Vector2 index out of range");
        }
    }

    const T& operator[](std::size_t index) const {
        switch (index) {
            case 0: return x;
            case 1: return y;
            default: throw std::out_of_range("Vector2 index out of range");
        }
    }

    // Vector operations
    T dot(const Vector2& other) const {
        return x * other.x + y * other.y;
    }

    T cross(const Vector2& other) const {
        return x * other.y - y * other.x;
    }

    T length_squared() const {
        return x * x + y * y;
    }

    T length() const {
        return std::sqrt(length_squared());
    }

    Vector2 normalize() const {
        T len = length();
        if (len == T(0)) {
            return Vector2(T(0));
        }
        return *this / len;
    }

    void normalize_in_place() {
        T len = length();
        if (len != T(0)) {
            *this /= len;
        }
    }

    // Distance functions
    T distance_to(const Vector2& other) const {
        return (*this - other).length();
    }

    T distance_squared_to(const Vector2& other) const {
        return (*this - other).length_squared();
    }

    // Utility functions
    bool is_zero(T epsilon = T(1e-6)) const {
        return std::abs(x) < epsilon && std::abs(y) < epsilon;
    }

    bool is_normalized(T epsilon = T(1e-6)) const {
        return std::abs(length() - T(1)) < epsilon;
    }

    // Static utility functions
    static Vector2 zero() { return Vector2(T(0)); }
    static Vector2 one() { return Vector2(T(1)); }
    static Vector2 unit_x() { return Vector2(T(1), T(0)); }
    static Vector2 unit_y() { return Vector2(T(0), T(1)); }

    // Interpolation
    static Vector2 lerp(const Vector2& a, const Vector2& b, T t) {
        return a + (b - a) * t;
    }
};

// Free function operators
template<typename T>
Vector2<T> operator*(T scalar, const Vector2<T>& vec) {
    return vec * scalar;
}

// Stream operators
template<typename T>
std::ostream& operator<<(std::ostream& os, const Vector2<T>& vec) {
    os << "(" << vec.x << ", " << vec.y << ")";
    return os;
}

// Type aliases
using Vector2f = Vector2<float>;
using Vector2d = Vector2<double>;
using Vector2i = Vector2<int>;

} // namespace math
} // namespace polygon_mesh