#pragma once

#include <cstdint>
#include <vector>
#include <limits>
#include <algorithm>
#include <polygon_mesh/math/vector3.hpp>
#include <polygon_mesh/math/vector2.hpp>

namespace polygon_mesh {
namespace core {

// Forward declarations
template<typename T> class Mesh;

// Basic type aliases
using VertexId = std::uint32_t;
using EdgeId = std::uint32_t;
using FaceId = std::uint32_t;
using MaterialId = std::uint32_t;

// Invalid ID constants
constexpr VertexId INVALID_VERTEX_ID = std::numeric_limits<VertexId>::max();
constexpr EdgeId INVALID_EDGE_ID = std::numeric_limits<EdgeId>::max();
constexpr FaceId INVALID_FACE_ID = std::numeric_limits<FaceId>::max();
constexpr MaterialId INVALID_MATERIAL_ID = std::numeric_limits<MaterialId>::max();

// Vertex structure
template<typename T>
struct Vertex {
    math::Vector3<T> position;
    math::Vector3<T> normal;
    math::Vector2<T> uv;
    VertexId id;

    Vertex() : position(0), normal(0), uv(0), id(INVALID_VERTEX_ID) {}
    
    Vertex(const math::Vector3<T>& pos) 
        : position(pos), normal(0), uv(0), id(INVALID_VERTEX_ID) {}
    
    Vertex(const math::Vector3<T>& pos, const math::Vector3<T>& norm) 
        : position(pos), normal(norm), uv(0), id(INVALID_VERTEX_ID) {}
    
    Vertex(const math::Vector3<T>& pos, const math::Vector3<T>& norm, const math::Vector2<T>& texture_coord)
        : position(pos), normal(norm), uv(texture_coord), id(INVALID_VERTEX_ID) {}

    // Comparison operators
    bool operator==(const Vertex& other) const {
        return position == other.position && 
               normal == other.normal && 
               uv == other.uv;
    }

    bool operator!=(const Vertex& other) const {
        return !(*this == other);
    }

    // Utility functions
    bool has_normal() const {
        return !normal.is_zero();
    }

    bool has_uv() const {
        return !uv.is_zero();
    }
};

// Edge structure
template<typename T>
struct Edge {
    VertexId v1, v2;                      // vertex indices
    std::vector<FaceId> adjacent_faces;   // adjacent faces
    EdgeId id;
    bool is_boundary;

    Edge() : v1(INVALID_VERTEX_ID), v2(INVALID_VERTEX_ID), 
             id(INVALID_EDGE_ID), is_boundary(false) {}
    
    Edge(VertexId vertex1, VertexId vertex2) 
        : v1(vertex1), v2(vertex2), id(INVALID_EDGE_ID), is_boundary(false) {
        // Ensure consistent ordering (smaller index first)
        if (v1 > v2) {
            std::swap(v1, v2);
        }
    }

    // Comparison operators
    bool operator==(const Edge& other) const {
        return v1 == other.v1 && v2 == other.v2;
    }

    bool operator!=(const Edge& other) const {
        return !(*this == other);
    }

    // Utility functions
    bool contains_vertex(VertexId vertex_id) const {
        return v1 == vertex_id || v2 == vertex_id;
    }

    VertexId get_other_vertex(VertexId vertex_id) const {
        if (v1 == vertex_id) return v2;
        if (v2 == vertex_id) return v1;
        return INVALID_VERTEX_ID;
    }

    bool is_manifold() const {
        return adjacent_faces.size() <= 2;
    }

    void update_boundary_status() {
        is_boundary = adjacent_faces.size() == 1;
    }
};

// Face structure
template<typename T>
struct Face {
    std::vector<VertexId> vertices;       // vertex indices (counter-clockwise)
    math::Vector3<T> normal;              // face normal
    MaterialId material_id;
    FaceId id;

    Face() : normal(0), material_id(INVALID_MATERIAL_ID), id(INVALID_FACE_ID) {}
    
    explicit Face(const std::vector<VertexId>& verts) 
        : vertices(verts), normal(0), material_id(INVALID_MATERIAL_ID), id(INVALID_FACE_ID) {}
    
    Face(const std::vector<VertexId>& verts, MaterialId mat_id)
        : vertices(verts), normal(0), material_id(mat_id), id(INVALID_FACE_ID) {}

    // Comparison operators
    bool operator==(const Face& other) const {
        return vertices == other.vertices && 
               normal == other.normal && 
               material_id == other.material_id;
    }

    bool operator!=(const Face& other) const {
        return !(*this == other);
    }

    // Utility functions
    std::size_t vertex_count() const {
        return vertices.size();
    }

    bool is_triangle() const {
        return vertices.size() == 3;
    }

    bool is_quad() const {
        return vertices.size() == 4;
    }

    bool contains_vertex(VertexId vertex_id) const {
        return std::find(vertices.begin(), vertices.end(), vertex_id) != vertices.end();
    }

    bool is_valid() const {
        return vertices.size() >= 3;
    }

    // Get edges of this face
    std::vector<std::pair<VertexId, VertexId>> get_edges() const {
        std::vector<std::pair<VertexId, VertexId>> edges;
        if (vertices.size() < 3) return edges;

        for (std::size_t i = 0; i < vertices.size(); ++i) {
            std::size_t next = (i + 1) % vertices.size();
            VertexId v1 = vertices[i];
            VertexId v2 = vertices[next];
            // Ensure consistent ordering
            if (v1 > v2) std::swap(v1, v2);
            edges.emplace_back(v1, v2);
        }
        return edges;
    }

    bool has_normal() const {
        return !normal.is_zero();
    }
};

// Bounding box structure
template<typename T>
struct BoundingBox {
    math::Vector3<T> min_point;
    math::Vector3<T> max_point;

    BoundingBox() {
        reset();
    }

    BoundingBox(const math::Vector3<T>& min_pt, const math::Vector3<T>& max_pt)
        : min_point(min_pt), max_point(max_pt) {}

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

    void expand(const BoundingBox& other) {
        expand(other.min_point);
        expand(other.max_point);
    }

    math::Vector3<T> center() const {
        return (min_point + max_point) * T(0.5);
    }

    math::Vector3<T> size() const {
        return max_point - min_point;
    }

    T volume() const {
        auto s = size();
        return s.x * s.y * s.z;
    }

    T surface_area() const {
        auto s = size();
        return T(2) * (s.x * s.y + s.y * s.z + s.z * s.x);
    }

    bool contains(const math::Vector3<T>& point) const {
        return point.x >= min_point.x && point.x <= max_point.x &&
               point.y >= min_point.y && point.y <= max_point.y &&
               point.z >= min_point.z && point.z <= max_point.z;
    }

    bool intersects(const BoundingBox& other) const {
        return min_point.x <= other.max_point.x && max_point.x >= other.min_point.x &&
               min_point.y <= other.max_point.y && max_point.y >= other.min_point.y &&
               min_point.z <= other.max_point.z && max_point.z >= other.min_point.z;
    }

    bool is_valid() const {
        return min_point.x <= max_point.x && 
               min_point.y <= max_point.y && 
               min_point.z <= max_point.z;
    }
};

// Type aliases for common instantiations
using Vertexf = Vertex<float>;
using Vertexd = Vertex<double>;
using Edgef = Edge<float>;
using Edged = Edge<double>;
using Facef = Face<float>;
using Faced = Face<double>;
using BoundingBoxf = BoundingBox<float>;
using BoundingBoxd = BoundingBox<double>;

} // namespace core
} // namespace polygon_mesh