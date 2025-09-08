#pragma once

#include <polygon_mesh/core/mesh.hpp>
#include <polygon_mesh/math/vector3.hpp>
#include <vector>
#include <cstddef>

namespace polygon_mesh {
namespace algorithms {

// Forward declarations
template<typename T> class Mesh;

// Mesh generation algorithms
namespace generation {
    
    // Create basic primitive meshes
    template<typename T>
    core::Mesh<T> create_triangle(const math::Vector3<T>& v1, 
                                  const math::Vector3<T>& v2, 
                                  const math::Vector3<T>& v3);

    template<typename T>
    core::Mesh<T> create_quad(const math::Vector3<T>& v1, 
                              const math::Vector3<T>& v2, 
                              const math::Vector3<T>& v3, 
                              const math::Vector3<T>& v4);

    template<typename T>
    core::Mesh<T> create_cube(const math::Vector3<T>& center, T size);

    template<typename T>
    core::Mesh<T> create_sphere(const math::Vector3<T>& center, T radius, 
                                std::size_t subdivisions = 3);

    template<typename T>
    core::Mesh<T> create_cylinder(const math::Vector3<T>& center, T radius, T height,
                                  std::size_t segments = 16);

    template<typename T>
    core::Mesh<T> create_plane(const math::Vector3<T>& center, 
                               const math::Vector3<T>& normal,
                               T width, T height,
                               std::size_t width_segments = 1,
                               std::size_t height_segments = 1);

} // namespace generation

// Mesh processing algorithms
namespace processing {

    // Normal computation
    template<typename T>
    void compute_vertex_normals(core::Mesh<T>& mesh);

    template<typename T>
    void compute_face_normals(core::Mesh<T>& mesh);

    // Smoothing algorithms
    template<typename T>
    void laplacian_smoothing(core::Mesh<T>& mesh, std::size_t iterations = 1, T lambda = T(0.5));

    template<typename T>
    void taubin_smoothing(core::Mesh<T>& mesh, std::size_t iterations = 1, 
                          T lambda = T(0.5), T mu = T(-0.53));

    // Mesh decimation (simplification)
    template<typename T>
    void quadric_decimation(core::Mesh<T>& mesh, T reduction_ratio);

    template<typename T>
    void edge_collapse_decimation(core::Mesh<T>& mesh, std::size_t target_triangles);

    // Mesh refinement
    template<typename T>
    void loop_subdivision(core::Mesh<T>& mesh, std::size_t levels = 1);

    template<typename T>
    void catmull_clark_subdivision(core::Mesh<T>& mesh, std::size_t levels = 1);

    // Mesh validation and repair
    template<typename T>
    bool validate_topology(const core::Mesh<T>& mesh);

    template<typename T>
    void remove_duplicate_vertices(core::Mesh<T>& mesh, T epsilon = T(1e-6));

    template<typename T>
    void remove_degenerate_faces(core::Mesh<T>& mesh, T min_area = T(1e-8));

    template<typename T>
    void flip_normals(core::Mesh<T>& mesh);

    template<typename T>
    void make_normals_consistent(core::Mesh<T>& mesh);

} // namespace processing

// Geometric analysis algorithms
namespace analysis {

    // Curvature computation
    template<typename T>
    std::vector<T> compute_mean_curvature(const core::Mesh<T>& mesh);

    template<typename T>
    std::vector<T> compute_gaussian_curvature(const core::Mesh<T>& mesh);

    // Quality metrics
    template<typename T>
    std::vector<T> compute_aspect_ratios(const core::Mesh<T>& mesh);

    template<typename T>
    std::vector<T> compute_triangle_areas(const core::Mesh<T>& mesh);

    template<typename T>
    std::vector<T> compute_triangle_angles(const core::Mesh<T>& mesh);

    // Topology analysis
    template<typename T>
    std::size_t compute_genus(const core::Mesh<T>& mesh);

    template<typename T>
    T compute_surface_area(const core::Mesh<T>& mesh);

    template<typename T>
    T compute_volume(const core::Mesh<T>& mesh);

    // Mesh statistics
    template<typename T>
    struct MeshStatistics {
        std::size_t vertex_count;
        std::size_t edge_count;
        std::size_t face_count;
        std::size_t triangle_count;
        std::size_t quad_count;
        std::size_t ngon_count;
        
        T min_edge_length;
        T max_edge_length;
        T avg_edge_length;
        
        T min_triangle_area;
        T max_triangle_area;
        T avg_triangle_area;
        
        T total_surface_area;
        T volume;
        
        core::BoundingBox<T> bounding_box;
    };

    template<typename T>
    MeshStatistics<T> compute_mesh_statistics(const core::Mesh<T>& mesh);

} // namespace analysis

// Collision detection and spatial queries
namespace spatial {

    // Ray-mesh intersection
    template<typename T>
    struct RayHit {
        bool hit;
        math::Vector3<T> point;
        math::Vector3<T> normal;
        T distance;
        core::FaceId face_id;
        math::Vector3<T> barycentric;
    };

    template<typename T>
    RayHit<T> ray_mesh_intersection(const math::Vector3<T>& ray_origin,
                                    const math::Vector3<T>& ray_direction,
                                    const core::Mesh<T>& mesh);

    // Distance queries
    template<typename T>
    T point_to_mesh_distance(const math::Vector3<T>& point, const core::Mesh<T>& mesh);

    template<typename T>
    math::Vector3<T> closest_point_on_mesh(const math::Vector3<T>& point, 
                                           const core::Mesh<T>& mesh);

    // Spatial partitioning structures
    template<typename T>
    class BVH {
    public:
        explicit BVH(const core::Mesh<T>& mesh);
        RayHit<T> ray_intersection(const math::Vector3<T>& ray_origin,
                                   const math::Vector3<T>& ray_direction) const;
    private:
        struct Node;
        std::vector<Node> nodes_;
        const core::Mesh<T>* mesh_;
    };

} // namespace spatial

} // namespace algorithms
} // namespace polygon_mesh