#pragma once

// Main algorithms header file - includes all algorithm modules

#include <polygon_mesh/algorithms/mesh_processing.hpp>

namespace polygon_mesh {
namespace algorithms {

// Convenience functions for common operations
template<typename T>
void process_mesh_pipeline(core::Mesh<T>& mesh, 
                           bool compute_normals = true,
                           bool remove_duplicates = true,
                           bool remove_degenerates = true,
                           bool validate = true) {
    using namespace processing;
    
    if (remove_duplicates) {
        remove_duplicate_vertices(mesh);
    }
    
    if (remove_degenerates) {
        remove_degenerate_faces(mesh);
    }
    
    if (compute_normals) {
        compute_face_normals(mesh);
        compute_vertex_normals(mesh);
    }
    
    if (validate) {
        if (!validate_topology(mesh)) {
            // Log warning about invalid topology
        }
    }
}

// Algorithm configuration structures
struct SmoothingConfig {
    enum Type { LAPLACIAN, TAUBIN } type = LAPLACIAN;
    std::size_t iterations = 1;
    float lambda = 0.5f;
    float mu = -0.53f;  // For Taubin smoothing
};

struct DecimationConfig {
    enum Type { QUADRIC, EDGE_COLLAPSE } type = QUADRIC;
    float reduction_ratio = 0.5f;
    std::size_t target_triangles = 0;
    bool preserve_boundaries = true;
    float quadric_threshold = 1e-6f;
};

struct SubdivisionConfig {
    enum Type { LOOP, CATMULL_CLARK } type = LOOP;
    std::size_t levels = 1;
    bool limit_surface = false;
};

} // namespace algorithms
} // namespace polygon_mesh