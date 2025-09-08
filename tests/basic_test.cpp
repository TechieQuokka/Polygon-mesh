#include <iostream>
#include <polygon_mesh/polygon_mesh.hpp>

using namespace polygon_mesh;
using namespace polygon_mesh::core;

int main() {
    std::cout << "Basic test starting..." << std::endl;
    
    try {
        // Create a mesh
        core::Meshf mesh;
        
        // Add vertices for a simple triangle
        auto v1 = mesh.add_vertex(math::Vector3f(0.0f, 0.0f, 0.0f));
        auto v2 = mesh.add_vertex(math::Vector3f(1.0f, 0.0f, 0.0f));
        auto v3 = mesh.add_vertex(math::Vector3f(0.5f, 1.0f, 0.0f));
        
        // Add a triangle face
        auto face_id = mesh.add_triangle(v1, v2, v3);
        
        // Basic validation
        if (mesh.vertex_count() != 3) {
            std::cerr << "Error: Expected 3 vertices, got " << mesh.vertex_count() << std::endl;
            return 1;
        }
        
        if (mesh.face_count() != 1) {
            std::cerr << "Error: Expected 1 face, got " << mesh.face_count() << std::endl;
            return 1;
        }
        
        std::cout << "Basic test passed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}