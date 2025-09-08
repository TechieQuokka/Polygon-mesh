#pragma once

// Main header file for the Polygon Mesh Library
// Include this file to access all library functionality

// Core components
#include <polygon_mesh/core/types.hpp>
#include <polygon_mesh/core/mesh.hpp>

// Math utilities
#include <polygon_mesh/math/vector2.hpp>
#include <polygon_mesh/math/vector3.hpp>
#include <polygon_mesh/math/matrix4.hpp>
#include <polygon_mesh/math/math_utils.hpp>

// Algorithm modules
#include <polygon_mesh/algorithms/algorithms.hpp>

// File I/O modules
#include <polygon_mesh/io/io.hpp>

// Utility modules
#include <polygon_mesh/utils/utils.hpp>

// Version information
#define POLYGON_MESH_VERSION_MAJOR 1
#define POLYGON_MESH_VERSION_MINOR 0
#define POLYGON_MESH_VERSION_PATCH 0
#define POLYGON_MESH_VERSION_STRING "1.0.0"

namespace polygon_mesh {
    
    // Version information
    inline constexpr int version_major() { return POLYGON_MESH_VERSION_MAJOR; }
    inline constexpr int version_minor() { return POLYGON_MESH_VERSION_MINOR; }
    inline constexpr int version_patch() { return POLYGON_MESH_VERSION_PATCH; }
    inline constexpr const char* version_string() { return POLYGON_MESH_VERSION_STRING; }
    
    // Library capabilities and features
    namespace features {
        inline constexpr bool has_matrix_math() { return true; }
        inline constexpr bool has_algorithms() { return true; }
        inline constexpr bool has_file_io() { return true; }
        inline constexpr bool has_obj_support() { return true; }
        inline constexpr bool has_ply_support() { return true; }
        inline constexpr bool has_threading_utils() { return true; }
        inline constexpr bool has_memory_pool() { return true; }
        inline constexpr bool has_profiling() { return true; }
        
        // Future features (not yet implemented)
        inline constexpr bool has_stl_support() { return false; }
        inline constexpr bool has_off_support() { return false; }
        inline constexpr bool has_gpu_acceleration() { return false; }
        inline constexpr bool has_subdivision_surfaces() { return false; }
        inline constexpr bool has_mesh_decimation() { return false; }
    }
    
    // Common type aliases for convenience
    namespace types {
        // Float precision types
        using Meshf = core::Mesh<float>;
        using Vector3f = math::Vector3<float>;
        using Vector2f = math::Vector2<float>;
        using Matrix4f = math::Matrix4<float>;
        using BoundingBoxf = core::BoundingBox<float>;
        
        // Double precision types
        using Meshd = core::Mesh<double>;
        using Vector3d = math::Vector3<double>;
        using Vector2d = math::Vector2<double>;
        using Matrix4d = math::Matrix4<double>;
        using BoundingBoxd = core::BoundingBox<double>;
        
        // Default types (single precision)
        using Mesh = Meshf;
        using Vector3 = Vector3f;
        using Vector2 = Vector2f;
        using Matrix4 = Matrix4f;
        using BoundingBox = BoundingBoxf;
    }
    
    // Import common types into main namespace for convenience
    using types::Mesh;
    using types::Vector3;
    using types::Vector2;
    using types::Matrix4;
    using types::BoundingBox;
    
} // namespace polygon_mesh

// Namespace alias for convenience
namespace pm = polygon_mesh;

// Global convenience macros
#define POLYGON_MESH_PROFILE_SCOPE(name) \
    polygon_mesh::utils::ProfileScope _pg_scope(name)

#define POLYGON_MESH_PROFILE_FUNCTION() \
    POLYGON_MESH_PROFILE_SCOPE(__FUNCTION__)

// End of file