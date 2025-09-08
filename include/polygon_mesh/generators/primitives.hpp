#pragma once

#include <polygon_mesh/core/mesh.hpp>
#include <polygon_mesh/math/math_utils.hpp>
#include <cmath>

namespace polygon_mesh {
namespace generators {

// Generate basic primitive meshes
template<typename T>
class Primitives {
public:
    // Create a triangle mesh
    static core::Mesh<T> triangle(T size = T(1)) {
        core::Mesh<T> mesh;
        
        T half_size = size * T(0.5);
        T height = size * T(0.866); // sqrt(3)/2
        
        // Add vertices
        auto v1 = mesh.add_vertex(core::Vertex<T>(math::Vector3<T>(-half_size, 0, -height / T(3))));
        auto v2 = mesh.add_vertex(core::Vertex<T>(math::Vector3<T>(half_size, 0, -height / T(3))));
        auto v3 = mesh.add_vertex(core::Vertex<T>(math::Vector3<T>(0, 0, height * T(2) / T(3))));
        
        // Add face
        mesh.add_triangle(v1, v2, v3);
        
        return mesh;
    }
    
    // Create a quad mesh
    static core::Mesh<T> quad(T width = T(1), T height = T(1)) {
        core::Mesh<T> mesh;
        
        T half_width = width * T(0.5);
        T half_height = height * T(0.5);
        
        // Add vertices
        auto v1 = mesh.add_vertex(core::Vertex<T>(math::Vector3<T>(-half_width, 0, -half_height)));
        auto v2 = mesh.add_vertex(core::Vertex<T>(math::Vector3<T>(half_width, 0, -half_height)));
        auto v3 = mesh.add_vertex(core::Vertex<T>(math::Vector3<T>(half_width, 0, half_height)));
        auto v4 = mesh.add_vertex(core::Vertex<T>(math::Vector3<T>(-half_width, 0, half_height)));
        
        // Add triangular faces (triangulate quad)
        mesh.add_triangle(v1, v2, v3);
        mesh.add_triangle(v1, v3, v4);
        
        return mesh;
    }
    
    // Create a cube mesh
    static core::Mesh<T> cube(T size = T(1)) {
        core::Mesh<T> mesh;
        
        T half_size = size * T(0.5);
        
        // Add vertices (8 corners of cube)
        std::vector<core::VertexId> vertices;
        vertices.push_back(mesh.add_vertex(core::Vertex<T>(math::Vector3<T>(-half_size, -half_size, -half_size)))); // 0
        vertices.push_back(mesh.add_vertex(core::Vertex<T>(math::Vector3<T>( half_size, -half_size, -half_size)))); // 1
        vertices.push_back(mesh.add_vertex(core::Vertex<T>(math::Vector3<T>( half_size,  half_size, -half_size)))); // 2
        vertices.push_back(mesh.add_vertex(core::Vertex<T>(math::Vector3<T>(-half_size,  half_size, -half_size)))); // 3
        vertices.push_back(mesh.add_vertex(core::Vertex<T>(math::Vector3<T>(-half_size, -half_size,  half_size)))); // 4
        vertices.push_back(mesh.add_vertex(core::Vertex<T>(math::Vector3<T>( half_size, -half_size,  half_size)))); // 5
        vertices.push_back(mesh.add_vertex(core::Vertex<T>(math::Vector3<T>( half_size,  half_size,  half_size)))); // 6
        vertices.push_back(mesh.add_vertex(core::Vertex<T>(math::Vector3<T>(-half_size,  half_size,  half_size)))); // 7
        
        // Add faces (12 triangles, 2 per face)
        // Front face (z = -half_size)
        mesh.add_triangle(vertices[0], vertices[1], vertices[2]);
        mesh.add_triangle(vertices[0], vertices[2], vertices[3]);
        
        // Back face (z = half_size)
        mesh.add_triangle(vertices[4], vertices[6], vertices[5]);
        mesh.add_triangle(vertices[4], vertices[7], vertices[6]);
        
        // Left face (x = -half_size)
        mesh.add_triangle(vertices[0], vertices[3], vertices[7]);
        mesh.add_triangle(vertices[0], vertices[7], vertices[4]);
        
        // Right face (x = half_size)
        mesh.add_triangle(vertices[1], vertices[5], vertices[6]);
        mesh.add_triangle(vertices[1], vertices[6], vertices[2]);
        
        // Bottom face (y = -half_size)
        mesh.add_triangle(vertices[0], vertices[4], vertices[5]);
        mesh.add_triangle(vertices[0], vertices[5], vertices[1]);
        
        // Top face (y = half_size)
        mesh.add_triangle(vertices[3], vertices[2], vertices[6]);
        mesh.add_triangle(vertices[3], vertices[6], vertices[7]);
        
        return mesh;
    }
    
    // Create a tetrahedron mesh
    static core::Mesh<T> tetrahedron(T size = T(1)) {
        core::Mesh<T> mesh;
        
        T a = size / std::sqrt(T(3));
        T h = size * std::sqrt(T(2.0/3.0));
        
        // Add vertices
        auto v1 = mesh.add_vertex(core::Vertex<T>(math::Vector3<T>(-a/2, 0, -a/(2*std::sqrt(T(3))))));
        auto v2 = mesh.add_vertex(core::Vertex<T>(math::Vector3<T>(a/2, 0, -a/(2*std::sqrt(T(3))))));
        auto v3 = mesh.add_vertex(core::Vertex<T>(math::Vector3<T>(0, 0, a/std::sqrt(T(3)))));
        auto v4 = mesh.add_vertex(core::Vertex<T>(math::Vector3<T>(0, h, a/(6*std::sqrt(T(3))))));
        
        // Add faces
        mesh.add_triangle(v1, v2, v3); // Base
        mesh.add_triangle(v1, v4, v2); // Side 1
        mesh.add_triangle(v2, v4, v3); // Side 2
        mesh.add_triangle(v3, v4, v1); // Side 3
        
        return mesh;
    }
    
    // Create a pyramid mesh (square base)
    static core::Mesh<T> pyramid(T base_size = T(1), T height = T(1)) {
        core::Mesh<T> mesh;
        
        T half_base = base_size * T(0.5);
        
        // Base vertices
        auto v1 = mesh.add_vertex(core::Vertex<T>(math::Vector3<T>(-half_base, 0, -half_base)));
        auto v2 = mesh.add_vertex(core::Vertex<T>(math::Vector3<T>( half_base, 0, -half_base)));
        auto v3 = mesh.add_vertex(core::Vertex<T>(math::Vector3<T>( half_base, 0,  half_base)));
        auto v4 = mesh.add_vertex(core::Vertex<T>(math::Vector3<T>(-half_base, 0,  half_base)));
        
        // Apex
        auto v5 = mesh.add_vertex(core::Vertex<T>(math::Vector3<T>(0, height, 0)));
        
        // Base triangles
        mesh.add_triangle(v1, v2, v3);
        mesh.add_triangle(v1, v3, v4);
        
        // Side triangles
        mesh.add_triangle(v1, v5, v2);
        mesh.add_triangle(v2, v5, v3);
        mesh.add_triangle(v3, v5, v4);
        mesh.add_triangle(v4, v5, v1);
        
        return mesh;
    }
};

// Convenience functions
template<typename T = float>
core::Mesh<T> create_triangle(T size = T(1)) {
    return Primitives<T>::triangle(size);
}

template<typename T = float>
core::Mesh<T> create_quad(T width = T(1), T height = T(1)) {
    return Primitives<T>::quad(width, height);
}

template<typename T = float>
core::Mesh<T> create_cube(T size = T(1)) {
    return Primitives<T>::cube(size);
}

template<typename T = float>
core::Mesh<T> create_tetrahedron(T size = T(1)) {
    return Primitives<T>::tetrahedron(size);
}

template<typename T = float>
core::Mesh<T> create_pyramid(T base_size = T(1), T height = T(1)) {
    return Primitives<T>::pyramid(base_size, height);
}

} // namespace generators
} // namespace polygon_mesh