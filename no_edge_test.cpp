#include <iostream>
#include <polygon_mesh/core/mesh.hpp>

using namespace polygon_mesh;

int main() {
    std::cout << "Testing mesh without edge updates..." << std::endl;
    
    try {
        // Create mesh
        core::Mesh<float> mesh;
        
        // Add vertices directly
        auto v1 = mesh.add_vertex(core::Vertex<float>(math::Vector3<float>(0.0f, 0.0f, 0.0f)));
        std::cout << "Added vertex 1: " << v1 << std::endl;
        
        auto v2 = mesh.add_vertex(core::Vertex<float>(math::Vector3<float>(1.0f, 0.0f, 0.0f)));
        std::cout << "Added vertex 2: " << v2 << std::endl;
        
        auto v3 = mesh.add_vertex(core::Vertex<float>(math::Vector3<float>(0.5f, 1.0f, 0.0f)));
        std::cout << "Added vertex 3: " << v3 << std::endl;
        
        std::cout << "Mesh has " << mesh.vertex_count() << " vertices" << std::endl;
        
        // Manually create and add face (without using add_triangle)
        std::vector<core::VertexId> face_vertices = {v1, v2, v3};
        core::Face<float> face(face_vertices);
        face.id = 0;
        
        std::cout << "Face created with " << face.vertex_count() << " vertices" << std::endl;
        std::cout << "Face vertices: " << face.vertices[0] << ", " << face.vertices[1] << ", " << face.vertices[2] << std::endl;
        
        // Test edge creation
        std::cout << "Testing edge creation..." << std::endl;
        auto edges = face.get_edges();
        std::cout << "Face has " << edges.size() << " edges" << std::endl;
        
        for (size_t i = 0; i < edges.size(); ++i) {
            std::cout << "Edge " << i << ": " << edges[i].first << " -> " << edges[i].second << std::endl;
        }
        
        std::cout << "Test completed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}