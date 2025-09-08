#include <iostream>
#include <fstream>
#include <polygon_mesh/core/mesh.hpp>
#include <polygon_mesh/io/ply_loader.hpp>

using namespace polygon_mesh;

// Extremely simple PLY save function
bool ultra_simple_ply_save(const std::string& filepath, const core::Mesh<float>& mesh) {
    std::ofstream file(filepath);
    if (!file.is_open()) return false;
    
    // Get data
    auto vertex_count = mesh.vertex_count();
    auto face_count = mesh.face_count();
    
    // Write header
    file << "ply\n";
    file << "format ascii 1.0\n";
    file << "element vertex " << vertex_count << "\n";
    file << "property float x\n";
    file << "property float y\n";
    file << "property float z\n";
    file << "element face " << face_count << "\n";
    file << "property list uchar int vertex_indices\n";
    file << "end_header\n";
    
    // Write vertices using array access
    const auto& vertices = mesh.vertices();
    for (std::size_t i = 0; i < vertex_count; ++i) {
        auto& pos = vertices[i].position;
        file << pos.x << " " << pos.y << " " << pos.z << "\n";
    }
    
    // Write faces using array access
    const auto& faces = mesh.faces();
    for (std::size_t i = 0; i < face_count; ++i) {
        auto& face = faces[i];
        file << face.vertices.size();
        for (std::size_t j = 0; j < face.vertices.size(); ++j) {
            file << " " << face.vertices[j];
        }
        file << "\n";
    }
    
    return true;
}

int main() {
    std::cout << "Simple PLY save test..." << std::endl;
    
    try {
        // Create mesh
        core::Mesh<float> mesh;
        
        // Add vertices without normals or UVs
        mesh.add_vertex(core::Vertex<float>(math::Vector3<float>(0.0f, 0.0f, 0.0f)));
        mesh.add_vertex(core::Vertex<float>(math::Vector3<float>(1.0f, 0.0f, 0.0f)));
        mesh.add_vertex(core::Vertex<float>(math::Vector3<float>(0.5f, 1.0f, 0.0f)));
        
        std::cout << "Created mesh with " << mesh.vertex_count() << " vertices" << std::endl;
        
        // Add a simple triangle
        mesh.add_triangle(0, 1, 2);
        std::cout << "Added triangle, mesh now has " << mesh.face_count() << " faces" << std::endl;
        
        // Use ultra simple PLY save
        bool result = ultra_simple_ply_save("ultra_simple.ply", mesh);
        std::cout << "Ultra simple PLY save: " << (result ? "SUCCESS" : "FAILED") << std::endl;
        
        // Test original PLY loader save
        std::cout << "Testing original PLY loader save..." << std::endl;
        result = io::PLYLoader::save("original_loader.ply", mesh);
        std::cout << "Original PLY loader save: " << (result ? "SUCCESS" : "FAILED") << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}