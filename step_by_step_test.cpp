#include <iostream>
#include <fstream>
#include <polygon_mesh/core/mesh.hpp>

using namespace polygon_mesh;

int main() {
    std::cout << "Step by step test..." << std::endl;
    
    try {
        // Create mesh with triangle
        core::Mesh<float> mesh;
        
        // Add vertices
        auto v1 = mesh.add_vertex(core::Vertex<float>(math::Vector3<float>(0.0f, 0.0f, 0.0f)));
        auto v2 = mesh.add_vertex(core::Vertex<float>(math::Vector3<float>(1.0f, 0.0f, 0.0f)));
        auto v3 = mesh.add_vertex(core::Vertex<float>(math::Vector3<float>(0.5f, 1.0f, 0.0f)));
        
        std::cout << "Added vertices: " << v1 << ", " << v2 << ", " << v3 << std::endl;
        
        // Add triangle
        auto face_id = mesh.add_triangle(v1, v2, v3);
        std::cout << "Added triangle with face ID: " << face_id << std::endl;
        
        std::cout << "Mesh has " << mesh.vertex_count() << " vertices and " << mesh.face_count() << " faces" << std::endl;
        
        // Test accessing vertices and faces safely
        std::cout << "Testing vertex access..." << std::endl;
        const auto& vertices = mesh.vertices();
        std::cout << "Vertices size: " << vertices.size() << std::endl;
        
        std::cout << "Testing face access..." << std::endl;
        const auto& faces = mesh.faces();
        std::cout << "Faces size: " << faces.size() << std::endl;
        
        if (!faces.empty()) {
            std::cout << "First face vertex count: " << faces[0].vertex_count() << std::endl;
            std::cout << "First face vertices: ";
            for (const auto& vid : faces[0].vertices) {
                std::cout << vid << " ";
            }
            std::cout << std::endl;
        }
        
        std::cout << "Step by step test completed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}