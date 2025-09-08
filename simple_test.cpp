#include <iostream>
#include <polygon_mesh/core/mesh.hpp>

using namespace polygon_mesh;

int main() {
    std::cout << "Testing basic mesh operations..." << std::endl;
    
    try {
        // Create mesh
        core::Mesh<float> mesh;
        std::cout << "Mesh created" << std::endl;
        
        // Create vertex
        core::Vertex<float> vertex(math::Vector3<float>(1.0f, 2.0f, 3.0f));
        std::cout << "Vertex created" << std::endl;
        
        // Add vertex
        auto vertex_id = mesh.add_vertex(vertex);
        std::cout << "Vertex added with ID: " << vertex_id << std::endl;
        
        // Check vertex count
        std::cout << "Vertex count: " << mesh.vertex_count() << std::endl;
        
        // Get vertices
        const auto& vertices = mesh.vertices();
        std::cout << "Got vertices vector, size: " << vertices.size() << std::endl;
        
        if (!vertices.empty()) {
            std::cout << "First vertex position: (" 
                      << vertices[0].position.x << ", " 
                      << vertices[0].position.y << ", " 
                      << vertices[0].position.z << ")" << std::endl;
            
            std::cout << "Has normal: " << (vertices[0].has_normal() ? "yes" : "no") << std::endl;
            std::cout << "Has UV: " << (vertices[0].has_uv() ? "yes" : "no") << std::endl;
        }
        
        std::cout << "Basic test completed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}