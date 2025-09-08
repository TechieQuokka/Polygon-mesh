#include <iostream>
#include <cassert>
#include <cmath>
#include <polygon_mesh/polygon_mesh.hpp>

using namespace polygon_mesh;
using namespace polygon_mesh::core;

void test_vector_operations() {
    std::cout << "Testing Vector3 operations..." << std::endl;
    
    // Basic construction
    math::Vector3f v1(1.0f, 2.0f, 3.0f);
    math::Vector3f v2(4.0f, 5.0f, 6.0f);
    
    // Arithmetic operations
    auto v3 = v1 + v2;
    assert(v3.x == 5.0f && v3.y == 7.0f && v3.z == 9.0f);
    
    auto v4 = v2 - v1;
    assert(v4.x == 3.0f && v4.y == 3.0f && v4.z == 3.0f);
    
    auto v5 = v1 * 2.0f;
    assert(v5.x == 2.0f && v5.y == 4.0f && v5.z == 6.0f);
    
    // Dot product
    float dot = v1.dot(v2);
    assert(std::abs(dot - 32.0f) < 1e-6f);
    
    // Cross product
    auto cross = v1.cross(v2);
    assert(cross.x == -3.0f && cross.y == 6.0f && cross.z == -3.0f);
    
    // Length
    float len = v1.length();
    assert(std::abs(len - std::sqrt(14.0f)) < 1e-6f);
    
    std::cout << "Vector3 tests passed!" << std::endl;
}

void test_vector2_operations() {
    std::cout << "Testing Vector2 operations..." << std::endl;
    
    math::Vector2f v1(3.0f, 4.0f);
    math::Vector2f v2(1.0f, 2.0f);
    
    // Basic operations
    auto v3 = v1 + v2;
    assert(v3.x == 4.0f && v3.y == 6.0f);
    
    // Length should be 5.0 for (3,4)
    float len = v1.length();
    assert(std::abs(len - 5.0f) < 1e-6f);
    
    // Normalize
    auto normalized = v1.normalize();
    assert(std::abs(normalized.length() - 1.0f) < 1e-6f);
    
    std::cout << "Vector2 tests passed!" << std::endl;
}

void test_mesh_creation() {
    std::cout << "Testing mesh creation..." << std::endl;
    
    core::Meshf mesh;
    
    // Add vertices
    auto v0 = mesh.add_vertex(math::Vector3f(0.0f, 0.0f, 0.0f));
    auto v1 = mesh.add_vertex(math::Vector3f(1.0f, 0.0f, 0.0f));
    auto v2 = mesh.add_vertex(math::Vector3f(0.5f, 1.0f, 0.0f));
    auto v3 = mesh.add_vertex(math::Vector3f(0.5f, 0.5f, 1.0f));
    
    assert(mesh.vertex_count() == 4);
    
    // Add faces
    auto f0 = mesh.add_triangle(v0, v1, v2);  // Base triangle
    auto f1 = mesh.add_triangle(v0, v1, v3);  // Side triangle
    auto f2 = mesh.add_triangle(v1, v2, v3);  // Side triangle
    auto f3 = mesh.add_triangle(v0, v2, v3);  // Side triangle
    
    assert(mesh.face_count() == 4);
    assert(mesh.edge_count() == 6);  // Tetrahedron has 6 edges
    
    std::cout << "Mesh creation tests passed!" << std::endl;
}

void test_mesh_geometry() {
    std::cout << "Testing mesh geometry..." << std::endl;
    
    core::Meshf mesh;
    
    // Create a simple triangle
    auto v0 = mesh.add_vertex(math::Vector3f(0.0f, 0.0f, 0.0f));
    auto v1 = mesh.add_vertex(math::Vector3f(1.0f, 0.0f, 0.0f));
    auto v2 = mesh.add_vertex(math::Vector3f(0.0f, 1.0f, 0.0f));
    
    auto face = mesh.add_triangle(v0, v1, v2);
    
    // Compute normals
    mesh.compute_normals();
    
    // Check face normal (should point in +Z direction)
    const auto& f = mesh.get_face(face);
    assert(std::abs(f.normal.z - 1.0f) < 1e-6f);
    assert(std::abs(f.normal.x) < 1e-6f);
    assert(std::abs(f.normal.y) < 1e-6f);
    
    // Surface area should be 0.5 (half of 1x1 square)
    float area = mesh.surface_area();
    assert(std::abs(area - 0.5f) < 1e-6f);
    
    // Bounding box
    const auto& bbox = mesh.bounding_box();
    assert(bbox.min_point.x == 0.0f && bbox.min_point.y == 0.0f);
    assert(bbox.max_point.x == 1.0f && bbox.max_point.y == 1.0f);
    
    std::cout << "Mesh geometry tests passed!" << std::endl;
}

void test_mesh_topology() {
    std::cout << "Testing mesh topology..." << std::endl;
    
    core::Meshf mesh;
    
    // Create a valid triangle
    auto v0 = mesh.add_vertex(math::Vector3f(0.0f, 0.0f, 0.0f));
    auto v1 = mesh.add_vertex(math::Vector3f(1.0f, 0.0f, 0.0f));
    auto v2 = mesh.add_vertex(math::Vector3f(0.0f, 1.0f, 0.0f));
    
    mesh.add_triangle(v0, v1, v2);
    
    // Should be valid
    assert(mesh.validate_topology());
    
    // Test edge information
    assert(mesh.edge_count() == 3);
    
    // All edges should be boundary edges (only one adjacent face)
    for (std::size_t i = 0; i < mesh.edges().size(); ++i) {
        const auto& edge = mesh.get_edge(i);
        assert(edge.is_boundary);
        assert(edge.adjacent_faces.size() == 1);
    }
    
    std::cout << "Mesh topology tests passed!" << std::endl;
}

void test_complex_mesh() {
    std::cout << "Testing complex mesh (cube)..." << std::endl;
    
    core::Meshf mesh;
    
    // Create cube vertices
    std::vector<VertexId> vertices;
    vertices.push_back(mesh.add_vertex(math::Vector3f(-1.0f, -1.0f, -1.0f))); // 0
    vertices.push_back(mesh.add_vertex(math::Vector3f( 1.0f, -1.0f, -1.0f))); // 1
    vertices.push_back(mesh.add_vertex(math::Vector3f( 1.0f,  1.0f, -1.0f))); // 2
    vertices.push_back(mesh.add_vertex(math::Vector3f(-1.0f,  1.0f, -1.0f))); // 3
    vertices.push_back(mesh.add_vertex(math::Vector3f(-1.0f, -1.0f,  1.0f))); // 4
    vertices.push_back(mesh.add_vertex(math::Vector3f( 1.0f, -1.0f,  1.0f))); // 5
    vertices.push_back(mesh.add_vertex(math::Vector3f( 1.0f,  1.0f,  1.0f))); // 6
    vertices.push_back(mesh.add_vertex(math::Vector3f(-1.0f,  1.0f,  1.0f))); // 7
    
    // Add cube faces (12 triangles, 2 per face)
    // Front face (z = -1)
    mesh.add_triangle(vertices[0], vertices[1], vertices[2]);
    mesh.add_triangle(vertices[0], vertices[2], vertices[3]);
    
    // Back face (z = 1)
    mesh.add_triangle(vertices[4], vertices[6], vertices[5]);
    mesh.add_triangle(vertices[4], vertices[7], vertices[6]);
    
    // Left face (x = -1)
    mesh.add_triangle(vertices[0], vertices[3], vertices[7]);
    mesh.add_triangle(vertices[0], vertices[7], vertices[4]);
    
    // Right face (x = 1)
    mesh.add_triangle(vertices[1], vertices[5], vertices[6]);
    mesh.add_triangle(vertices[1], vertices[6], vertices[2]);
    
    // Top face (y = 1)
    mesh.add_triangle(vertices[3], vertices[2], vertices[6]);
    mesh.add_triangle(vertices[3], vertices[6], vertices[7]);
    
    // Bottom face (y = -1)
    mesh.add_triangle(vertices[0], vertices[4], vertices[5]);
    mesh.add_triangle(vertices[0], vertices[5], vertices[1]);
    
    assert(mesh.vertex_count() == 8);
    assert(mesh.face_count() == 12);
    assert(mesh.edge_count() == 18);
    
    // Compute normals
    mesh.compute_normals();
    
    // Surface area of cube with side length 2 should be 24
    float area = mesh.surface_area();
    assert(std::abs(area - 24.0f) < 1e-5f);
    
    // Volume of cube with side length 2 should be 8
    float volume = mesh.volume();
    assert(std::abs(volume - 8.0f) < 1e-5f);
    
    // Bounding box
    const auto& bbox = mesh.bounding_box();
    assert(std::abs(bbox.min_point.x + 1.0f) < 1e-6f);
    assert(std::abs(bbox.max_point.x - 1.0f) < 1e-6f);
    assert(std::abs(bbox.volume() - 8.0f) < 1e-5f);
    
    std::cout << "Complex mesh tests passed!" << std::endl;
}

void test_error_handling() {
    std::cout << "Testing error handling..." << std::endl;
    
    core::Meshf mesh;
    
    // Test invalid vertex access
    try {
        mesh.get_vertex(999);
        assert(false); // Should not reach here
    } catch (const std::out_of_range&) {
        // Expected
    }
    
    // Test invalid face creation
    try {
        mesh.add_triangle(0, 1, 2); // Vertices don't exist
        assert(false); // Should not reach here  
    } catch (const std::out_of_range&) {
        // Expected
    }
    
    // Test degenerate face
    try {
        mesh.add_face({0}); // Only 1 vertex
        assert(false); // Should not reach here
    } catch (const std::invalid_argument&) {
        // Expected
    }
    
    std::cout << "Error handling tests passed!" << std::endl;
}

int main() {
    std::cout << "=== Polygon Mesh Library - Comprehensive Test Suite ===" << std::endl;
    
    try {
        test_vector_operations();
        test_vector2_operations();
        test_mesh_creation();
        test_mesh_geometry();
        test_mesh_topology();
        test_complex_mesh();
        test_error_handling();
        
        std::cout << "\n=== All tests passed successfully! ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}