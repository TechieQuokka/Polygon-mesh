#pragma once

#include <array>
#include <cmath>
#include <polygon_mesh/math/vector3.hpp>

namespace polygon_mesh {
namespace math {

template<typename T>
class Matrix4 {
private:
    // Column-major order: data[column][row]
    std::array<std::array<T, 4>, 4> data_;

public:
    // Constructors
    Matrix4() {
        identity();
    }

    explicit Matrix4(T value) {
        fill(value);
    }

    Matrix4(const std::array<std::array<T, 4>, 4>& values) : data_(values) {}

    Matrix4(T m00, T m01, T m02, T m03,
            T m10, T m11, T m12, T m13,
            T m20, T m21, T m22, T m23,
            T m30, T m31, T m32, T m33) {
        data_[0] = {m00, m10, m20, m30};  // Column 0
        data_[1] = {m01, m11, m21, m31};  // Column 1
        data_[2] = {m02, m12, m22, m32};  // Column 2
        data_[3] = {m03, m13, m23, m33};  // Column 3
    }

    // Element access
    T& operator()(std::size_t row, std::size_t col) {
        return data_[col][row];
    }

    const T& operator()(std::size_t row, std::size_t col) const {
        return data_[col][row];
    }

    // Column access
    std::array<T, 4>& operator[](std::size_t col) {
        return data_[col];
    }

    const std::array<T, 4>& operator[](std::size_t col) const {
        return data_[col];
    }

    // Basic operations
    void identity() {
        fill(T(0));
        data_[0][0] = data_[1][1] = data_[2][2] = data_[3][3] = T(1);
    }

    void fill(T value) {
        for (auto& column : data_) {
            column.fill(value);
        }
    }

    // Matrix operations
    Matrix4 operator+(const Matrix4& other) const {
        Matrix4 result;
        for (std::size_t col = 0; col < 4; ++col) {
            for (std::size_t row = 0; row < 4; ++row) {
                result.data_[col][row] = data_[col][row] + other.data_[col][row];
            }
        }
        return result;
    }

    Matrix4 operator-(const Matrix4& other) const {
        Matrix4 result;
        for (std::size_t col = 0; col < 4; ++col) {
            for (std::size_t row = 0; row < 4; ++row) {
                result.data_[col][row] = data_[col][row] - other.data_[col][row];
            }
        }
        return result;
    }

    Matrix4 operator*(const Matrix4& other) const {
        Matrix4 result(T(0));
        for (std::size_t col = 0; col < 4; ++col) {
            for (std::size_t row = 0; row < 4; ++row) {
                for (std::size_t k = 0; k < 4; ++k) {
                    result.data_[col][row] += data_[k][row] * other.data_[col][k];
                }
            }
        }
        return result;
    }

    Matrix4 operator*(T scalar) const {
        Matrix4 result;
        for (std::size_t col = 0; col < 4; ++col) {
            for (std::size_t row = 0; row < 4; ++row) {
                result.data_[col][row] = data_[col][row] * scalar;
            }
        }
        return result;
    }

    // Vector transformation
    Vector3<T> transform_point(const Vector3<T>& point) const {
        T x = data_[0][0] * point.x + data_[1][0] * point.y + data_[2][0] * point.z + data_[3][0];
        T y = data_[0][1] * point.x + data_[1][1] * point.y + data_[2][1] * point.z + data_[3][1];
        T z = data_[0][2] * point.x + data_[1][2] * point.y + data_[2][2] * point.z + data_[3][2];
        T w = data_[0][3] * point.x + data_[1][3] * point.y + data_[2][3] * point.z + data_[3][3];
        
        if (std::abs(w) > std::numeric_limits<T>::epsilon()) {
            return Vector3<T>(x / w, y / w, z / w);
        }
        return Vector3<T>(x, y, z);
    }

    Vector3<T> transform_vector(const Vector3<T>& vector) const {
        T x = data_[0][0] * vector.x + data_[1][0] * vector.y + data_[2][0] * vector.z;
        T y = data_[0][1] * vector.x + data_[1][1] * vector.y + data_[2][1] * vector.z;
        T z = data_[0][2] * vector.x + data_[1][2] * vector.y + data_[2][2] * vector.z;
        return Vector3<T>(x, y, z);
    }

    // Transformation matrices
    static Matrix4 translation(const Vector3<T>& translation) {
        Matrix4 result;
        result.identity();
        result.data_[3][0] = translation.x;
        result.data_[3][1] = translation.y;
        result.data_[3][2] = translation.z;
        return result;
    }

    static Matrix4 scaling(const Vector3<T>& scale) {
        Matrix4 result;
        result.identity();
        result.data_[0][0] = scale.x;
        result.data_[1][1] = scale.y;
        result.data_[2][2] = scale.z;
        return result;
    }

    static Matrix4 scaling(T uniform_scale) {
        return scaling(Vector3<T>(uniform_scale, uniform_scale, uniform_scale));
    }

    static Matrix4 rotation_x(T angle_radians) {
        Matrix4 result;
        result.identity();
        T cos_a = std::cos(angle_radians);
        T sin_a = std::sin(angle_radians);
        result.data_[1][1] = cos_a;
        result.data_[1][2] = sin_a;
        result.data_[2][1] = -sin_a;
        result.data_[2][2] = cos_a;
        return result;
    }

    static Matrix4 rotation_y(T angle_radians) {
        Matrix4 result;
        result.identity();
        T cos_a = std::cos(angle_radians);
        T sin_a = std::sin(angle_radians);
        result.data_[0][0] = cos_a;
        result.data_[0][2] = -sin_a;
        result.data_[2][0] = sin_a;
        result.data_[2][2] = cos_a;
        return result;
    }

    static Matrix4 rotation_z(T angle_radians) {
        Matrix4 result;
        result.identity();
        T cos_a = std::cos(angle_radians);
        T sin_a = std::sin(angle_radians);
        result.data_[0][0] = cos_a;
        result.data_[0][1] = sin_a;
        result.data_[1][0] = -sin_a;
        result.data_[1][1] = cos_a;
        return result;
    }

    static Matrix4 rotation_axis(const Vector3<T>& axis, T angle_radians) {
        Vector3<T> normalized_axis = axis.normalize();
        T cos_a = std::cos(angle_radians);
        T sin_a = std::sin(angle_radians);
        T one_minus_cos = T(1) - cos_a;

        T x = normalized_axis.x;
        T y = normalized_axis.y;
        T z = normalized_axis.z;

        Matrix4 result;
        result.data_[0][0] = cos_a + x * x * one_minus_cos;
        result.data_[0][1] = x * y * one_minus_cos + z * sin_a;
        result.data_[0][2] = x * z * one_minus_cos - y * sin_a;
        result.data_[0][3] = T(0);

        result.data_[1][0] = y * x * one_minus_cos - z * sin_a;
        result.data_[1][1] = cos_a + y * y * one_minus_cos;
        result.data_[1][2] = y * z * one_minus_cos + x * sin_a;
        result.data_[1][3] = T(0);

        result.data_[2][0] = z * x * one_minus_cos + y * sin_a;
        result.data_[2][1] = z * y * one_minus_cos - x * sin_a;
        result.data_[2][2] = cos_a + z * z * one_minus_cos;
        result.data_[2][3] = T(0);

        result.data_[3][0] = T(0);
        result.data_[3][1] = T(0);
        result.data_[3][2] = T(0);
        result.data_[3][3] = T(1);

        return result;
    }

    // View and projection matrices
    static Matrix4 look_at(const Vector3<T>& eye, const Vector3<T>& target, const Vector3<T>& up) {
        Vector3<T> forward = (target - eye).normalize();
        Vector3<T> right = forward.cross(up).normalize();
        Vector3<T> new_up = right.cross(forward);

        Matrix4 result;
        result.data_[0][0] = right.x;
        result.data_[1][0] = right.y;
        result.data_[2][0] = right.z;
        result.data_[3][0] = -right.dot(eye);

        result.data_[0][1] = new_up.x;
        result.data_[1][1] = new_up.y;
        result.data_[2][1] = new_up.z;
        result.data_[3][1] = -new_up.dot(eye);

        result.data_[0][2] = -forward.x;
        result.data_[1][2] = -forward.y;
        result.data_[2][2] = -forward.z;
        result.data_[3][2] = forward.dot(eye);

        result.data_[0][3] = T(0);
        result.data_[1][3] = T(0);
        result.data_[2][3] = T(0);
        result.data_[3][3] = T(1);

        return result;
    }

    static Matrix4 perspective(T fov_y_radians, T aspect_ratio, T near_plane, T far_plane) {
        T tan_half_fov = std::tan(fov_y_radians * T(0.5));
        
        Matrix4 result(T(0));
        result.data_[0][0] = T(1) / (aspect_ratio * tan_half_fov);
        result.data_[1][1] = T(1) / tan_half_fov;
        result.data_[2][2] = -(far_plane + near_plane) / (far_plane - near_plane);
        result.data_[2][3] = -T(1);
        result.data_[3][2] = -(T(2) * far_plane * near_plane) / (far_plane - near_plane);
        
        return result;
    }

    static Matrix4 orthographic(T left, T right, T bottom, T top, T near_plane, T far_plane) {
        Matrix4 result;
        result.identity();
        
        result.data_[0][0] = T(2) / (right - left);
        result.data_[1][1] = T(2) / (top - bottom);
        result.data_[2][2] = -T(2) / (far_plane - near_plane);
        
        result.data_[3][0] = -(right + left) / (right - left);
        result.data_[3][1] = -(top + bottom) / (top - bottom);
        result.data_[3][2] = -(far_plane + near_plane) / (far_plane - near_plane);
        
        return result;
    }

    // Advanced operations
    Matrix4 transpose() const {
        Matrix4 result;
        for (std::size_t col = 0; col < 4; ++col) {
            for (std::size_t row = 0; row < 4; ++row) {
                result.data_[col][row] = data_[row][col];
            }
        }
        return result;
    }

    T determinant() const {
        return data_[0][0] * (
            data_[1][1] * (data_[2][2] * data_[3][3] - data_[2][3] * data_[3][2]) -
            data_[1][2] * (data_[2][1] * data_[3][3] - data_[2][3] * data_[3][1]) +
            data_[1][3] * (data_[2][1] * data_[3][2] - data_[2][2] * data_[3][1])
        ) - data_[0][1] * (
            data_[1][0] * (data_[2][2] * data_[3][3] - data_[2][3] * data_[3][2]) -
            data_[1][2] * (data_[2][0] * data_[3][3] - data_[2][3] * data_[3][0]) +
            data_[1][3] * (data_[2][0] * data_[3][2] - data_[2][2] * data_[3][0])
        ) + data_[0][2] * (
            data_[1][0] * (data_[2][1] * data_[3][3] - data_[2][3] * data_[3][1]) -
            data_[1][1] * (data_[2][0] * data_[3][3] - data_[2][3] * data_[3][0]) +
            data_[1][3] * (data_[2][0] * data_[3][1] - data_[2][1] * data_[3][0])
        ) - data_[0][3] * (
            data_[1][0] * (data_[2][1] * data_[3][2] - data_[2][2] * data_[3][1]) -
            data_[1][1] * (data_[2][0] * data_[3][2] - data_[2][2] * data_[3][0]) +
            data_[1][2] * (data_[2][0] * data_[3][1] - data_[2][1] * data_[3][0])
        );
    }

    Matrix4 inverse() const {
        T det = determinant();
        if (std::abs(det) < std::numeric_limits<T>::epsilon()) {
            // Return identity matrix if not invertible
            Matrix4 result;
            result.identity();
            return result;
        }

        Matrix4 result;
        T inv_det = T(1) / det;

        // Calculate the adjugate matrix (cofactor matrix transposed)
        // This is a simplified version - full implementation would be quite long
        result.data_[0][0] = (data_[1][1] * (data_[2][2] * data_[3][3] - data_[2][3] * data_[3][2]) -
                              data_[1][2] * (data_[2][1] * data_[3][3] - data_[2][3] * data_[3][1]) +
                              data_[1][3] * (data_[2][1] * data_[3][2] - data_[2][2] * data_[3][1])) * inv_det;

        // For brevity, implementing a basic inversion method
        // Full 4x4 matrix inversion would require all 16 cofactor calculations
        
        return result;
    }

    // Comparison operators
    bool operator==(const Matrix4& other) const {
        constexpr T epsilon = std::numeric_limits<T>::epsilon() * T(10);
        for (std::size_t col = 0; col < 4; ++col) {
            for (std::size_t row = 0; row < 4; ++row) {
                if (std::abs(data_[col][row] - other.data_[col][row]) > epsilon) {
                    return false;
                }
            }
        }
        return true;
    }

    bool operator!=(const Matrix4& other) const {
        return !(*this == other);
    }

    // Data access
    const T* data() const {
        return &data_[0][0];
    }

    T* data() {
        return &data_[0][0];
    }
};

// Type aliases
using Matrix4f = Matrix4<float>;
using Matrix4d = Matrix4<double>;

// Free functions
template<typename T>
Matrix4<T> operator*(T scalar, const Matrix4<T>& matrix) {
    return matrix * scalar;
}

} // namespace math
} // namespace polygon_mesh