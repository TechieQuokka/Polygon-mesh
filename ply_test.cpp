#include <iostream>
#include <polygon_mesh/core/mesh.hpp>
#include <polygon_mesh/io/ply_loader.hpp>

using namespace polygon_mesh;

int main() {
    std::cout << "Testing PLY save/load..." << std::endl;
    
    try {
        // Create simple mesh
        core::Mesh<float> mesh;
        
        // Add vertices
        mesh.add_vertex(core::Vertex<float>(math::Vector3<float>(0.0f, 0.0f, 0.0f)));
        mesh.add_vertex(core::Vertex<float>(math::Vector3<float>(1.0f, 0.0f, 0.0f)));
        mesh.add_vertex(core::Vertex<float>(math::Vector3<float>(0.0f, 1.0f, 0.0f)));
        
        std::cout << "Created mesh with " << mesh.vertex_count() << " vertices" << std::endl;
        
        // Test header writing first
        std::cout << "Testing PLY header writing..." << std::endl;
        std::ofstream test_file("test_header.ply");
        if (test_file.is_open()) {
            test_file << "ply\n";
            test_file << "format ascii 1.0\n";
            test_file << "element vertex " << mesh.vertex_count() << "\n";
            test_file << "property float x\n";
            test_file << "property float y\n";  
            test_file << "property float z\n";
            test_file << "element face 0\n";
            test_file << "property list uchar int vertex_indices\n";
            test_file << "end_header\n";
            
            // Write vertices
            const auto& vertices = mesh.vertices();
            for (const auto& vertex : vertices) {
                test_file << vertex.position.x << " " << vertex.position.y << " " << vertex.position.z << "\n";
            }
            
            test_file.close();
            std::cout << "Manual PLY file written successfully" << std::endl;
        }
        
        // Now try PLY loader save
        std::cout << "Testing PLY loader save..." << std::endl;
        bool result = io::PLYLoader::save("test_ply_loader.ply", mesh, io::PLYLoader::Format::ASCII);
        std::cout << "PLY loader save result: " << (result ? "success" : "failed") << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}