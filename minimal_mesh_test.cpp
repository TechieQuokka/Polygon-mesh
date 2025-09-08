#include <iostream>
#include <polygon_mesh/core/mesh.hpp>

using namespace polygon_mesh;

int main() {
    std::cout << "Minimal mesh test..." << std::endl;
    
    try {
        std::cout << "Creating mesh..." << std::endl;
        core::Mesh<float> mesh;
        
        std::cout << "Adding first vertex..." << std::endl;
        auto v1 = mesh.add_vertex(core::Vertex<float>(math::Vector3<float>(0.0f, 0.0f, 0.0f)));
        std::cout << "Added vertex 1: " << v1 << std::endl;
        
        std::cout << "Adding second vertex..." << std::endl;
        auto v2 = mesh.add_vertex(core::Vertex<float>(math::Vector3<float>(1.0f, 0.0f, 0.0f)));
        std::cout << "Added vertex 2: " << v2 << std::endl;
        
        std::cout << "Adding third vertex..." << std::endl;
        auto v3 = mesh.add_vertex(core::Vertex<float>(math::Vector3<float>(0.5f, 1.0f, 0.0f)));
        std::cout << "Added vertex 3: " << v3 << std::endl;
        
        std::cout << "Mesh now has " << mesh.vertex_count() << " vertices" << std::endl;
        
        std::cout << "Attempting to add triangle..." << std::endl;
        auto face_id = mesh.add_triangle(v1, v2, v3);
        std::cout << "Added triangle with face ID: " << face_id << std::endl;
        
        std::cout << "Mesh now has " << mesh.face_count() << " faces" << std::endl;
        
        std::cout << "Minimal mesh test completed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}