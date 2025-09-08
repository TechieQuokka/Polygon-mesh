#pragma once

#include <cmath>
#include <algorithm>
#include <type_traits>
#include <cstdint>

namespace polygon_mesh {
namespace math {

// Constants
template<typename T>
constexpr T pi() { return static_cast<T>(3.14159265358979323846); }

template<typename T>
constexpr T two_pi() { return static_cast<T>(2) * pi<T>(); }

template<typename T>
constexpr T half_pi() { return pi<T>() / static_cast<T>(2); }

template<typename T>
constexpr T epsilon() { return static_cast<T>(1e-6); }

// Utility functions
template<typename T>
constexpr T clamp(T value, T min_val, T max_val) {
    return std::max(min_val, std::min(value, max_val));
}

template<typename T>
constexpr T lerp(T a, T b, T t) {
    return a + t * (b - a);
}

template<typename T>
constexpr T degrees_to_radians(T degrees) {
    return degrees * pi<T>() / static_cast<T>(180);
}

template<typename T>
constexpr T radians_to_degrees(T radians) {
    return radians * static_cast<T>(180) / pi<T>();
}

template<typename T>
bool approximately_equal(T a, T b, T epsilon = epsilon<T>()) {
    return std::abs(a - b) < epsilon;
}

template<typename T>
bool approximately_zero(T value, T epsilon = epsilon<T>()) {
    return std::abs(value) < epsilon;
}

template<typename T>
T smoothstep(T edge0, T edge1, T x) {
    T t = clamp((x - edge0) / (edge1 - edge0), T(0), T(1));
    return t * t * (T(3) - T(2) * t);
}

template<typename T>
constexpr T square(T x) {
    return x * x;
}

template<typename T>
constexpr int sign(T x) {
    return (x > T(0)) - (x < T(0));
}

// Safe division
template<typename T>
T safe_divide(T numerator, T denominator, T default_value = T(0)) {
    return approximately_zero(denominator) ? default_value : numerator / denominator;
}

// Safe square root
template<typename T>
T safe_sqrt(T x) {
    return std::sqrt(std::max(x, T(0)));
}

// Safe inverse square root (fast approximation for float)
template<typename T>
T fast_inv_sqrt(T x) {
    if constexpr (std::is_same_v<T, float>) {
        // Quake's fast inverse square root
        float x2 = x * 0.5f;
        std::uint32_t i = *(std::uint32_t*)&x;
        i = 0x5f3759df - (i >> 1);
        x = *(float*)&i;
        x = x * (1.5f - x2 * x * x);  // Newton iteration
        return x;
    } else {
        return T(1) / safe_sqrt(x);
    }
}

// Min/Max for multiple values
template<typename T>
constexpr T min3(T a, T b, T c) {
    return std::min({a, b, c});
}

template<typename T>
constexpr T max3(T a, T b, T c) {
    return std::max({a, b, c});
}

template<typename T>
constexpr T min4(T a, T b, T c, T d) {
    return std::min({a, b, c, d});
}

template<typename T>
constexpr T max4(T a, T b, T c, T d) {
    return std::max({a, b, c, d});
}

// Wrap angle to [0, 2π)
template<typename T>
T wrap_angle_2pi(T angle) {
    angle = std::fmod(angle, two_pi<T>());
    if (angle < T(0)) {
        angle += two_pi<T>();
    }
    return angle;
}

// Wrap angle to [-π, π)
template<typename T>
T wrap_angle_pi(T angle) {
    angle = wrap_angle_2pi(angle);
    if (angle > pi<T>()) {
        angle -= two_pi<T>();
    }
    return angle;
}

// Barycentric coordinates
template<typename T>
struct BarycentricCoords {
    T u, v, w;
    
    BarycentricCoords() : u(0), v(0), w(0) {}
    BarycentricCoords(T u, T v, T w) : u(u), v(v), w(w) {}
    
    bool is_valid() const {
        return u >= T(0) && v >= T(0) && w >= T(0) && 
               approximately_equal(u + v + w, T(1));
    }
};

// Random number utilities (simple LCG)
class SimpleRandom {
private:
    std::uint32_t state_;

public:
    explicit SimpleRandom(std::uint32_t seed = 1) : state_(seed) {}
    
    std::uint32_t next() {
        state_ = state_ * 1664525u + 1013904223u;
        return state_;
    }
    
    float next_float() {
        return static_cast<float>(next()) / static_cast<float>(0xFFFFFFFF);
    }
    
    double next_double() {
        return static_cast<double>(next()) / static_cast<double>(0xFFFFFFFF);
    }
    
    template<typename T>
    T range(T min_val, T max_val) {
        if constexpr (std::is_floating_point_v<T>) {
            T t = static_cast<T>(next_double());
            return lerp(min_val, max_val, t);
        } else {
            return min_val + static_cast<T>(next() % static_cast<std::uint32_t>(max_val - min_val + 1));
        }
    }
};

} // namespace math
} // namespace polygon_mesh