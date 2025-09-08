#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <stdexcept>
#include <polygon_mesh/core/types.hpp>
#include <polygon_mesh/math/math_utils.hpp>

namespace polygon_mesh {
namespace core {

template<typename T>
class Mesh {
private:
    std::vector<Vertex<T>> vertices_;
    std::vector<Edge<T>> edges_;
    std::vector<Face<T>> faces_;
    
    // Hash function for vertex pair
    struct PairHash {
        std::size_t operator()(const std::pair<VertexId, VertexId>& p) const {
            return std::hash<VertexId>()(p.first) ^ (std::hash<VertexId>()(p.second) << 1);
        }
    };
    
    // Topology tracking
    std::unordered_map<std::pair<VertexId, VertexId>, EdgeId, PairHash> edge_map_;
    bool topology_valid_;
    mutable bool bounding_box_dirty_;
    mutable BoundingBox<T> bounding_box_;

public:
    // Constructors
    Mesh() : topology_valid_(true), bounding_box_dirty_(true) {}
    
    // Copy constructor
    Mesh(const Mesh& other) = default;
    Mesh& operator=(const Mesh& other) = default;
    
    // Move constructor
    Mesh(Mesh&& other) noexcept = default;
    Mesh& operator=(Mesh&& other) noexcept = default;

    // Destructor
    ~Mesh() = default;

    // Vertex operations
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

    VertexId add_vertex(const math::Vector3<T>& position, const math::Vector3<T>& normal) {
        return add_vertex(Vertex<T>(position, normal));
    }

    VertexId add_vertex(const math::Vector3<T>& position, const math::Vector3<T>& normal, 
                        const math::Vector2<T>& uv) {
        return add_vertex(Vertex<T>(position, normal, uv));
    }

    // Face operations
    FaceId add_face(const std::vector<VertexId>& vertex_indices) {
        if (vertex_indices.size() < 3) {
            throw std::invalid_argument("Face must have at least 3 vertices");
        }

        // Validate vertex indices
        for (VertexId vid : vertex_indices) {
            if (vid >= vertices_.size()) {
                throw std::out_of_range("Invalid vertex index");
            }
        }

        FaceId face_id = static_cast<FaceId>(faces_.size());
        Face<T> face(vertex_indices);
        face.id = face_id;
        faces_.push_back(face);

        // Edge topology update disabled for stability
        // TODO: Implement stable edge topology management
        // update_edges_for_face(face_id);
        
        return face_id;
    }

    FaceId add_triangle(VertexId v1, VertexId v2, VertexId v3) {
        return add_face({v1, v2, v3});
    }

    FaceId add_quad(VertexId v1, VertexId v2, VertexId v3, VertexId v4) {
        return add_face({v1, v2, v3, v4});
    }

    // Accessors
    const std::vector<Vertex<T>>& vertices() const { return vertices_; }
    const std::vector<Edge<T>>& edges() const { return edges_; }
    const std::vector<Face<T>>& faces() const { return faces_; }
    
    std::size_t vertex_count() const { return vertices_.size(); }
    std::size_t edge_count() const { return edges_.size(); }
    std::size_t face_count() const { return faces_.size(); }

    // Element access
    const Vertex<T>& get_vertex(VertexId id) const {
        if (id >= vertices_.size()) {
            throw std::out_of_range("Invalid vertex ID");
        }
        return vertices_[id];
    }

    Vertex<T>& get_vertex(VertexId id) {
        if (id >= vertices_.size()) {
            throw std::out_of_range("Invalid vertex ID");
        }
        bounding_box_dirty_ = true;
        return vertices_[id];
    }

    const Face<T>& get_face(FaceId id) const {
        if (id >= faces_.size()) {
            throw std::out_of_range("Invalid face ID");
        }
        return faces_[id];
    }

    Face<T>& get_face(FaceId id) {
        if (id >= faces_.size()) {
            throw std::out_of_range("Invalid face ID");
        }
        return faces_[id];
    }

    const Edge<T>& get_edge(EdgeId id) const {
        if (id >= edges_.size()) {
            throw std::out_of_range("Invalid edge ID");
        }
        return edges_[id];
    }

    // Geometry operations
    void compute_face_normals() {
        for (auto& face : faces_) {
            compute_face_normal(face);
        }
    }

    void compute_vertex_normals() {
        // Reset all vertex normals
        for (auto& vertex : vertices_) {
            vertex.normal = math::Vector3<T>(0);
        }

        // Accumulate face normals
        for (const auto& face : faces_) {
            if (!face.has_normal()) continue;

            for (VertexId vid : face.vertices) {
                vertices_[vid].normal += face.normal;
            }
        }

        // Normalize
        for (auto& vertex : vertices_) {
            vertex.normal.normalize_in_place();
        }
    }

    void compute_normals() {
        compute_face_normals();
        compute_vertex_normals();
    }

    // Bounding box
    const BoundingBox<T>& bounding_box() const {
        if (bounding_box_dirty_) {
            compute_bounding_box();
            bounding_box_dirty_ = false;
        }
        return bounding_box_;
    }

    // Topology validation
    bool validate_topology() const {
        // Check for duplicate vertices
        for (std::size_t i = 0; i < vertices_.size(); ++i) {
            for (std::size_t j = i + 1; j < vertices_.size(); ++j) {
                if (vertices_[i].position == vertices_[j].position) {
                    return false; // Duplicate vertex
                }
            }
        }

        // Check face validity
        for (const auto& face : faces_) {
            if (!face.is_valid()) return false;
            
            // Check for duplicate vertex indices in face
            std::unordered_set<VertexId> unique_vertices;
            for (VertexId vid : face.vertices) {
                if (unique_vertices.count(vid) > 0) {
                    return false; // Duplicate vertex in face
                }
                unique_vertices.insert(vid);
            }
        }

        // Check edge manifoldness
        for (const auto& edge : edges_) {
            if (!edge.is_manifold()) {
                return false; // Non-manifold edge
            }
        }

        return true;
    }

    // Utility functions
    bool empty() const {
        return vertices_.empty() && faces_.empty();
    }

    void clear() {
        vertices_.clear();
        edges_.clear();
        faces_.clear();
        edge_map_.clear();
        topology_valid_ = true;
        bounding_box_dirty_ = true;
    }

    void reserve_vertices(std::size_t count) {
        vertices_.reserve(count);
    }

    void reserve_faces(std::size_t count) {
        faces_.reserve(count);
    }

    // Statistics
    T surface_area() const {
        T area = T(0);
        for (const auto& face : faces_) {
            if (face.is_triangle()) {
                area += compute_triangle_area(face);
            } else {
                // For non-triangles, triangulate and sum
                for (std::size_t i = 1; i < face.vertices.size() - 1; ++i) {
                    Face<T> triangle({face.vertices[0], face.vertices[i], face.vertices[i + 1]});
                    area += compute_triangle_area(triangle);
                }
            }
        }
        return area;
    }

    T volume() const {
        // Using divergence theorem (assumes closed mesh)
        T volume = T(0);
        for (const auto& face : faces_) {
            if (face.vertex_count() < 3) continue;

            const auto& v0 = vertices_[face.vertices[0]].position;
            for (std::size_t i = 1; i < face.vertex_count() - 1; ++i) {
                const auto& v1 = vertices_[face.vertices[i]].position;
                const auto& v2 = vertices_[face.vertices[i + 1]].position;
                
                // Compute signed volume of tetrahedron
                volume += v0.dot(v1.cross(v2));
            }
        }
        return std::abs(volume) / T(6);
    }

private:
    void update_edges_for_face(FaceId face_id) {
        const auto& face = faces_[face_id];
        auto face_edges = face.get_edges();

        for (const auto& edge_pair : face_edges) {
            auto it = edge_map_.find(edge_pair);
            if (it == edge_map_.end()) {
                // Create new edge
                EdgeId edge_id = static_cast<EdgeId>(edges_.size());
                Edge<T> edge;
                edge.v1 = edge_pair.first;
                edge.v2 = edge_pair.second;
                edge.id = edge_id;
                edge.adjacent_faces.push_back(face_id);
                edge.is_boundary = true;  // Will be updated later
                edges_.push_back(edge);
                edge_map_[edge_pair] = edge_id;
            } else {
                // Update existing edge
                edges_[it->second].adjacent_faces.push_back(face_id);
                edges_[it->second].update_boundary_status();
            }
        }
    }

    void compute_face_normal(Face<T>& face) const {
        if (face.vertex_count() < 3) {
            face.normal = math::Vector3<T>(0);
            return;
        }

        // Use Newell's method for robust normal computation
        math::Vector3<T> normal(0);
        for (std::size_t i = 0; i < face.vertex_count(); ++i) {
            std::size_t next = (i + 1) % face.vertex_count();
            const auto& v1 = vertices_[face.vertices[i]].position;
            const auto& v2 = vertices_[face.vertices[next]].position;
            
            normal.x += (v1.y - v2.y) * (v1.z + v2.z);
            normal.y += (v1.z - v2.z) * (v1.x + v2.x);
            normal.z += (v1.x - v2.x) * (v1.y + v2.y);
        }
        
        face.normal = normal.normalize();
    }

    T compute_triangle_area(const Face<T>& face) const {
        if (!face.is_triangle()) return T(0);
        
        const auto& v0 = vertices_[face.vertices[0]].position;
        const auto& v1 = vertices_[face.vertices[1]].position;
        const auto& v2 = vertices_[face.vertices[2]].position;
        
        return (v1 - v0).cross(v2 - v0).length() * T(0.5);
    }

    void compute_bounding_box() const {
        bounding_box_.reset();
        for (const auto& vertex : vertices_) {
            bounding_box_.expand(vertex.position);
        }
    }
};

// Type aliases
using Meshf = Mesh<float>;
using Meshd = Mesh<double>;

} // namespace core
} // namespace polygon_mesh