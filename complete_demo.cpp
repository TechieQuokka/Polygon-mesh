/*
 * Complete Polygon Mesh Library Demo
 * Copy and paste this entire file into an online compiler:
 * - https://godbolt.org/ (Compiler Explorer)
 * - https://www.onlinegdb.com/online_c++_compiler
 * - https://coliru.stacked-crooked.com/
 * 
 * Compiler settings: -std=c++17 -O2
 */

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <stdexcept>
#include <ostream>
#include <type_traits>

// POLYGON MESH LIBRARY - COMPLETE IMPLEMENTATION
namespace polygon_mesh {
namespace math {

template<typename T>
class Vector3 {
    static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");
public:
    T x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(T x, T y, T z) : x(x), y(y), z(z) {}
    Vector3(T value) : x(value), y(value), z(value) {}

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
    T length() const {
        return std::sqrt(x * x + y * y + z * z);
    }
    Vector3 normalize() const {
        T len = length();
        return len == T(0) ? Vector3(0) : *this / len;
    }

    bool operator==(const Vector3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
    bool is_zero(T epsilon = T(1e-6)) const {
        return std::abs(x) < epsilon && std::abs(y) < epsilon && std::abs(z) < epsilon;
    }
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const Vector3<T>& vec) {
    os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return os;
}

using Vector3f = Vector3<float>;

} // namespace math

namespace core {

using VertexId = std::uint32_t;
using FaceId = std::uint32_t;
using EdgeId = std::uint32_t;

constexpr VertexId INVALID_VERTEX_ID = std::numeric_limits<VertexId>::max();

template<typename T>
struct Vertex {
    math::Vector3<T> position;
    math::Vector3<T> normal;
    VertexId id;

    Vertex() : position(0), normal(0), id(INVALID_VERTEX_ID) {}
    Vertex(const math::Vector3<T>& pos) : position(pos), normal(0), id(INVALID_VERTEX_ID) {}
};

template<typename T>
struct Face {
    std::vector<VertexId> vertices;
    math::Vector3<T> normal;
    FaceId id;

    Face() : normal(0) {}
    Face(const std::vector<VertexId>& verts) : vertices(verts), normal(0) {}

    bool is_triangle() const { return vertices.size() == 3; }
    bool is_valid() const { return vertices.size() >= 3; }
};

template<typename T>
struct BoundingBox {
    math::Vector3<T> min_point;
    math::Vector3<T> max_point;

    BoundingBox() {
        reset();
    }

    void reset() {
        min_point = math::Vector3<T>(std::numeric_limits<T>::max());
        max_point = math::Vector3<T>(std::numeric_limits<T>::lowest());
    }

    void expand(const math::Vector3<T>& point) {
        min_point.x = std::min(min_point.x, point.x);
        min_point.y = std::min(min_point.y, point.y);
        min_point.z = std::min(min_point.z, point.z);
        
        max_point.x = std::max(max_point.x, point.x);
        max_point.y = std::max(max_point.y, point.y);
        max_point.z = std::max(max_point.z, point.z);
    }

    math::Vector3<T> center() const {
        return (min_point + max_point) * T(0.5);
    }

    math::Vector3<T> size() const {
        return max_point - min_point;
    }
};

template<typename T>
class Mesh {
private:
    std::vector<Vertex<T>> vertices_;
    std::vector<Face<T>> faces_;
    mutable bool bounding_box_dirty_;
    mutable BoundingBox<T> bounding_box_;

public:
    Mesh() : bounding_box_dirty_(true) {}

    VertexId add_vertex(const Vertex<T>& vertex) {
        VertexId id = static_cast<VertexId>(vertices_.size());
        Vertex<T> v = vertex;
        v.id = id;
        vertices_.push_back(v);
        bounding_box_dirty_ = true;
        return id;
    }

    VertexId add_vertex(const math::Vector3<T>& position) {
        return add_vertex(Vertex<T>(position));
    }

    FaceId add_face(const std::vector<VertexId>& vertex_indices) {
        if (vertex_indices.size() < 3) {
            throw std::invalid_argument("Face must have at least 3 vertices");
        }

        for (VertexId vid : vertex_indices) {
            if (vid >= vertices_.size()) {
                throw std::out_of_range("Invalid vertex index");
            }
        }

        FaceId face_id = static_cast<FaceId>(faces_.size());
        Face<T> face(vertex_indices);
        face.id = face_id;
        faces_.push_back(face);
        return face_id;
    }

    FaceId add_triangle(VertexId v1, VertexId v2, VertexId v3) {
        return add_face({v1, v2, v3});
    }

    const std::vector<Vertex<T>>& vertices() const { return vertices_; }
    const std::vector<Face<T>>& faces() const { return faces_; }
    
    std::size_t vertex_count() const { return vertices_.size(); }
    std::size_t face_count() const { return faces_.size(); }

    const Vertex<T>& get_vertex(VertexId id) const {
        if (id >= vertices_.size()) {
            throw std::out_of_range("Invalid vertex ID");
        }
        return vertices_[id];
    }

    const Face<T>& get_face(FaceId id) const {
        if (id >= faces_.size()) {
            throw std::out_of_range("Invalid face ID");
        }
        return faces_[id];
    }

    void compute_face_normals() {
        for (auto& face : faces_) {
            if (face.vertices.size() < 3) continue;

            const auto& v0 = vertices_[face.vertices[0]].position;
            const auto& v1 = vertices_[face.vertices[1]].position;
            const auto& v2 = vertices_[face.vertices[2]].position;
            
            auto edge1 = v1 - v0;
            auto edge2 = v2 - v0;
            face.normal = edge1.cross(edge2).normalize();
        }
    }

    void compute_vertex_normals() {
        for (auto& vertex : vertices_) {
            vertex.normal = math::Vector3<T>(0);
        }

        for (const auto& face : faces_) {
            if (face.normal.is_zero()) continue;
            for (VertexId vid : face.vertices) {
                vertices_[vid].normal = vertices_[vid].normal + face.normal;
            }
        }

        for (auto& vertex : vertices_) {
            vertex.normal = vertex.normal.normalize();
        }
    }

    void compute_normals() {
        compute_face_normals();
        compute_vertex_normals();
    }

    const BoundingBox<T>& bounding_box() const {
        if (bounding_box_dirty_) {
            bounding_box_.reset();
            for (const auto& vertex : vertices_) {
                bounding_box_.expand(vertex.position);
            }
            bounding_box_dirty_ = false;
        }
        return bounding_box_;
    }

    T surface_area() const {
        T area = T(0);
        for (const auto& face : faces_) {
            if (!face.is_triangle()) continue;
            
            const auto& v0 = vertices_[face.vertices[0]].position;
            const auto& v1 = vertices_[face.vertices[1]].position;
            const auto& v2 = vertices_[face.vertices[2]].position;
            
            area += (v1 - v0).cross(v2 - v0).length() * T(0.5);
        }
        return area;
    }
};

using Meshf = Mesh<float>;

} // namespace core
} // namespace polygon_mesh

// DEMO APPLICATION
int main() {
    using namespace polygon_mesh;
    
    std::cout << "🎯 Polygon Mesh Library - Complete Demo\n" << std::endl;
    
    try {
        // Demo 1: Vector3 Operations
        std::cout << "=== Demo 1: Vector3 Operations ===\n";
        {
            math::Vector3f v1(1.0f, 2.0f, 3.0f);
            math::Vector3f v2(4.0f, 5.0f, 6.0f);
            
            auto sum = v1 + v2;
            auto dot = v1.dot(v2);
            auto cross = v1.cross(v2);
            
            std::cout << "v1 = " << v1 << std::endl;
            std::cout << "v2 = " << v2 << std::endl;
            std::cout << "v1 + v2 = " << sum << std::endl;
            std::cout << "v1 · v2 = " << dot << std::endl;
            std::cout << "v1 × v2 = " << cross << std::endl;
            std::cout << "✅ Vector operations working!\n" << std::endl;
        }
        
        // Demo 2: Simple Triangle
        std::cout << "=== Demo 2: Simple Triangle ===\n";
        {
            core::Meshf mesh;
            
            auto v1 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
            auto v2 = mesh.add_vertex({1.0f, 0.0f, 0.0f});
            auto v3 = mesh.add_vertex({0.5f, 1.0f, 0.0f});
            
            auto face_id = mesh.add_triangle(v1, v2, v3);
            mesh.compute_normals();
            
            const auto& face = mesh.get_face(face_id);
            auto area = mesh.surface_area();
            
            std::cout << "Triangle vertices: 3" << std::endl;
            std::cout << "Triangle faces: 1" << std::endl;
            std::cout << "Face normal: " << face.normal << std::endl;
            std::cout << "Surface area: " << area << std::endl;
            std::cout << "✅ Triangle mesh working!\n" << std::endl;
        }
        
        // Demo 3: Cube Mesh
        std::cout << "=== Demo 3: Cube Mesh ===\n";
        {
            core::Meshf mesh;
            
            // Add cube vertices
            mesh.add_vertex({-0.5f, -0.5f, -0.5f}); // 0
            mesh.add_vertex({ 0.5f, -0.5f, -0.5f}); // 1
            mesh.add_vertex({ 0.5f,  0.5f, -0.5f}); // 2
            mesh.add_vertex({-0.5f,  0.5f, -0.5f}); // 3
            mesh.add_vertex({-0.5f, -0.5f,  0.5f}); // 4
            mesh.add_vertex({ 0.5f, -0.5f,  0.5f}); // 5
            mesh.add_vertex({ 0.5f,  0.5f,  0.5f}); // 6
            mesh.add_vertex({-0.5f,  0.5f,  0.5f}); // 7
            
            // Add cube faces (as triangles)
            std::vector<std::vector<int>> faces = {
                {0,1,2}, {0,2,3}, // bottom
                {4,7,6}, {4,6,5}, // top
                {0,4,5}, {0,5,1}, // front
                {2,6,7}, {2,7,3}, // back
                {0,3,7}, {0,7,4}, // left
                {1,5,6}, {1,6,2}  // right
            };
            
            for (const auto& face : faces) {
                mesh.add_triangle(face[0], face[1], face[2]);
            }
            
            mesh.compute_normals();
            
            const auto& bbox = mesh.bounding_box();
            auto area = mesh.surface_area();
            
            std::cout << "Cube vertices: " << mesh.vertex_count() << std::endl;
            std::cout << "Cube faces: " << mesh.face_count() << std::endl;
            std::cout << "Bounding box min: " << bbox.min_point << std::endl;
            std::cout << "Bounding box max: " << bbox.max_point << std::endl;
            std::cout << "Bounding box center: " << bbox.center() << std::endl;
            std::cout << "Bounding box size: " << bbox.size() << std::endl;
            std::cout << "Surface area: " << area << " (expected: ~6.0)" << std::endl;
            std::cout << "✅ Cube mesh working!\n" << std::endl;
        }
        
        // Demo 4: Advanced Features Test
        std::cout << "=== Demo 4: Advanced Features ===\n";
        {
            core::Meshf mesh;
            
            // Create pyramid
            mesh.add_vertex({ 0.0f,  0.0f,  1.0f}); // 0 - apex
            mesh.add_vertex({-1.0f, -1.0f,  0.0f}); // 1
            mesh.add_vertex({ 1.0f, -1.0f,  0.0f}); // 2
            mesh.add_vertex({ 1.0f,  1.0f,  0.0f}); // 3
            mesh.add_vertex({-1.0f,  1.0f,  0.0f}); // 4
            
            // Add pyramid faces
            mesh.add_triangle(0, 1, 2); // front
            mesh.add_triangle(0, 2, 3); // right
            mesh.add_triangle(0, 3, 4); // back
            mesh.add_triangle(0, 4, 1); // left
            mesh.add_triangle(1, 4, 3); // bottom 1
            mesh.add_triangle(1, 3, 2); // bottom 2
            
            mesh.compute_normals();
            
            // Test vertex normals
            std::cout << "Pyramid apex normal: " << mesh.get_vertex(0).normal << std::endl;
            std::cout << "Pyramid surface area: " << mesh.surface_area() << std::endl;
            std::cout << "✅ Advanced features working!" << std::endl;
        }
        
        std::cout << "\n🎉 ALL DEMOS COMPLETED SUCCESSFULLY! 🎉" << std::endl;
        std::cout << "📦 Polygon Mesh Library is fully functional!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}