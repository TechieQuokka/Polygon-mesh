#pragma once

#include <cmath>
#include <ostream>
#include <type_traits>
#include <algorithm>

namespace polygon_mesh {
namespace math {

template<typename T>
class Vector3 {
    static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");

public:
    T x, y, z;

    // Constructors
    Vector3() : x(0), y(0), z(0) {}
    Vector3(T x, T y, T z) : x(x), y(y), z(z) {}
    Vector3(T value) : x(value), y(value), z(value) {}

    // Copy and assignment
    Vector3(const Vector3& other) = default;
    Vector3& operator=(const Vector3& other) = default;
    Vector3(Vector3&& other) noexcept = default;
    Vector3& operator=(Vector3&& other) noexcept = default;

    // Arithmetic operators
    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    Vector3 operator*(T scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    Vector3 operator/(T scalar) const {
        return Vector3(x / scalar, y / scalar, z / scalar);
    }

    Vector3 operator-() const {
        return Vector3(-x, -y, -z);
    }

    // Compound assignment operators
    Vector3& operator+=(const Vector3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Vector3& operator-=(const Vector3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    Vector3& operator*=(T scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    Vector3& operator/=(T scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    // Comparison operators
    bool operator==(const Vector3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator!=(const Vector3& other) const {
        return !(*this == other);
    }

    // Element access
    T& operator[](std::size_t index) {
        switch (index) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: throw std::out_of_range("Vector3 index out of range");
        }
    }

    const T& operator[](std::size_t index) const {
        switch (index) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: throw std::out_of_range("Vector3 index out of range");
        }
    }

    // Vector operations
    T dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    Vector3 cross(const Vector3& other) const {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    T length_squared() const {
        return x * x + y * y + z * z;
    }

    T length() const {
        return std::sqrt(length_squared());
    }

    Vector3 normalize() const {
        T len = length();
        if (len == T(0)) {
            return Vector3(T(0));
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
    T distance_to(const Vector3& other) const {
        return (*this - other).length();
    }

    T distance_squared_to(const Vector3& other) const {
        return (*this - other).length_squared();
    }

    // Utility functions
    bool is_zero(T epsilon = T(1e-6)) const {
        return std::abs(x) < epsilon && std::abs(y) < epsilon && std::abs(z) < epsilon;
    }

    bool is_normalized(T epsilon = T(1e-6)) const {
        return std::abs(length() - T(1)) < epsilon;
    }

    // Static utility functions
    static Vector3 zero() { return Vector3(T(0)); }
    static Vector3 one() { return Vector3(T(1)); }
    static Vector3 unit_x() { return Vector3(T(1), T(0), T(0)); }
    static Vector3 unit_y() { return Vector3(T(0), T(1), T(0)); }
    static Vector3 unit_z() { return Vector3(T(0), T(0), T(1)); }

    // Interpolation
    static Vector3 lerp(const Vector3& a, const Vector3& b, T t) {
        return a + (b - a) * t;
    }

    static Vector3 slerp(const Vector3& a, const Vector3& b, T t) {
        T dot_product = a.dot(b);
        dot_product = std::clamp(dot_product, T(-1), T(1));
        
        T theta = std::acos(dot_product) * t;
        Vector3 relative_vec = (b - a * dot_product).normalize();
        
        return a * std::cos(theta) + relative_vec * std::sin(theta);
    }
};

// Free function operators
template<typename T>
Vector3<T> operator*(T scalar, const Vector3<T>& vec) {
    return vec * scalar;
}

// Stream operators
template<typename T>
std::ostream& operator<<(std::ostream& os, const Vector3<T>& vec) {
    os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return os;
}

// Type aliases
using Vector3f = Vector3<float>;
using Vector3d = Vector3<double>;
using Vector3i = Vector3<int>;

} // namespace math
} // namespace polygon_mesh