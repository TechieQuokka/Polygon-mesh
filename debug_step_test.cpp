#include <iostream>
#include <polygon_mesh/core/mesh.hpp>

using namespace polygon_mesh;

int main() {
    std::cout << "Debug step test..." << std::endl;
    
    try {
        // Create mesh
        core::Mesh<float> mesh;
        
        // Add vertices
        auto v1 = mesh.add_vertex(core::Vertex<float>(math::Vector3<float>(0.0f, 0.0f, 0.0f)));
        auto v2 = mesh.add_vertex(core::Vertex<float>(math::Vector3<float>(1.0f, 0.0f, 0.0f)));
        auto v3 = mesh.add_vertex(core::Vertex<float>(math::Vector3<float>(0.5f, 1.0f, 0.0f)));
        
        // Add triangle
        mesh.add_triangle(v1, v2, v3);
        
        std::cout << "Mesh created successfully" << std::endl;
        
        // Step 1: Get vertex count
        std::cout << "Step 1: Getting vertex count..." << std::endl;
        auto vertex_count = mesh.vertex_count();
        std::cout << "Vertex count: " << vertex_count << std::endl;
        
        // Step 2: Get face count
        std::cout << "Step 2: Getting face count..." << std::endl;
        auto face_count = mesh.face_count();
        std::cout << "Face count: " << face_count << std::endl;
        
        // Step 3: Get vertices reference
        std::cout << "Step 3: Getting vertices reference..." << std::endl;
        const auto& vertices = mesh.vertices();
        std::cout << "Got vertices reference, size: " << vertices.size() << std::endl;
        
        // Step 4: Access first vertex
        std::cout << "Step 4: Accessing first vertex..." << std::endl;
        if (!vertices.empty()) {
            const auto& vertex = vertices[0];
            std::cout << "First vertex position: " << vertex.position.x << ", " << vertex.position.y << ", " << vertex.position.z << std::endl;
        }
        
        // Step 5: Get faces reference
        std::cout << "Step 5: Getting faces reference..." << std::endl;
        const auto& faces = mesh.faces();
        std::cout << "Got faces reference, size: " << faces.size() << std::endl;
        
        // Step 6: Access first face
        std::cout << "Step 6: Accessing first face..." << std::endl;
        if (!faces.empty()) {
            const auto& face = faces[0];
            std::cout << "First face vertex count: " << face.vertices.size() << std::endl;
        }
        
        std::cout << "All steps completed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}