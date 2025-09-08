#include <iostream>
#include <fstream>
#include <polygon_mesh/core/mesh.hpp>

using namespace polygon_mesh;

int main() {
    std::cout << "Minimal PLY test..." << std::endl;
    
    try {
        // Create simple mesh
        core::Mesh<float> mesh;
        mesh.add_vertex(core::Vertex<float>(math::Vector3<float>(1.0f, 2.0f, 3.0f)));
        
        std::cout << "Mesh created with " << mesh.vertex_count() << " vertices" << std::endl;
        std::cout << "Mesh has " << mesh.face_count() << " faces" << std::endl;
        
        // Test accessing vertices
        const auto& vertices = mesh.vertices();
        std::cout << "Got vertices reference, size: " << vertices.size() << std::endl;
        
        if (!vertices.empty()) {
            const auto& vertex = vertices[0];
            std::cout << "First vertex position: (" << vertex.position.x << ", " << vertex.position.y << ", " << vertex.position.z << ")" << std::endl;
            std::cout << "Has normal: " << (vertex.has_normal() ? "yes" : "no") << std::endl;
            std::cout << "Has UV: " << (vertex.has_uv() ? "yes" : "no") << std::endl;
        }
        
        // Test accessing faces
        const auto& faces = mesh.faces();
        std::cout << "Got faces reference, size: " << faces.size() << std::endl;
        
        // Test manual PLY writing
        std::cout << "Writing minimal PLY file..." << std::endl;
        std::ofstream file("minimal.ply");
        if (file.is_open()) {
            file << "ply\n";
            file << "format ascii 1.0\n";
            file << "element vertex " << vertices.size() << "\n";
            file << "property float x\n";
            file << "property float y\n";
            file << "property float z\n";
            file << "element face 0\n";
            file << "property list uchar int vertex_indices\n";
            file << "end_header\n";
            
            for (const auto& vertex : vertices) {
                file << vertex.position.x << " " << vertex.position.y << " " << vertex.position.z << "\n";
            }
            
            file.close();
            std::cout << "Minimal PLY file written successfully" << std::endl;
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}