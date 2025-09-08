#include <polygon_mesh/core/mesh.hpp>
#include <polygon_mesh/generators/primitives.hpp>
#include <iostream>
#include <iomanip>

using namespace polygon_mesh;

void test_primitive(const std::string& name, const core::Mesh<float>& mesh) {
    std::cout << "\n--- " << name << " ---\n";
    std::cout << "Vertices: " << mesh.vertex_count() << "\n";
    std::cout << "Faces: " << mesh.face_count() << "\n";
    
    // Compute and display bounding box
    auto bbox = mesh.bounding_box();
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Bounding box: ";
    std::cout << "min(" << bbox.min_point.x << ", " << bbox.min_point.y << ", " << bbox.min_point.z << ") ";
    std::cout << "max(" << bbox.max_point.x << ", " << bbox.max_point.y << ", " << bbox.max_point.z << ")\n";
    
    // Display first few vertices
    const auto& vertices = mesh.vertices();
    std::cout << "First few vertices:\n";
    std::size_t max_vertices = std::min(static_cast<std::size_t>(4), vertices.size());
    for (std::size_t i = 0; i < max_vertices; ++i) {
        const auto& pos = vertices[i].position;
        std::cout << "  v" << i << ": (" << pos.x << ", " << pos.y << ", " << pos.z << ")\n";
    }
    
    // Display first few faces
    const auto& faces = mesh.faces();
    std::cout << "First few faces:\n";
    std::size_t max_faces = std::min(static_cast<std::size_t>(3), faces.size());
    for (std::size_t i = 0; i < max_faces; ++i) {
        const auto& face = faces[i];
        std::cout << "  f" << i << ": ";
        for (std::size_t j = 0; j < face.vertices.size(); ++j) {
            std::cout << face.vertices[j];
            if (j < face.vertices.size() - 1) std::cout << " ";
        }
        std::cout << "\n";
    }
}

int main() {
    std::cout << "=== Primitive Mesh Generators Demo ===\n";
    
    try {
        // Test triangle
        auto triangle = generators::create_triangle(2.0f);
        test_primitive("Triangle", triangle);
        
        // Test quad
        auto quad = generators::create_quad(2.0f, 1.5f);
        test_primitive("Quad", quad);
        
        // Test cube
        auto cube = generators::create_cube(1.0f);
        test_primitive("Cube", cube);
        
        // Test tetrahedron
        auto tetrahedron = generators::create_tetrahedron(1.0f);
        test_primitive("Tetrahedron", tetrahedron);
        
        // Test pyramid
        auto pyramid = generators::create_pyramid(2.0f, 1.5f);
        test_primitive("Pyramid", pyramid);
        
        // Test different template parameter
        std::cout << "\n--- Double Precision Cube ---\n";
        auto double_cube = generators::create_cube<double>(1.0);
        std::cout << "Double precision cube vertices: " << double_cube.vertex_count() << "\n";
        std::cout << "Double precision cube faces: " << double_cube.face_count() << "\n";
        
        std::cout << "\n=== Primitives demo completed successfully! ===\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}