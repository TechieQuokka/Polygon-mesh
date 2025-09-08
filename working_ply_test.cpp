#include <iostream>
#include <fstream>
#include <polygon_mesh/core/mesh.hpp>

using namespace polygon_mesh;

// Ultra safe PLY save
bool safe_ply_save(const std::string& filepath, const core::Mesh<float>& mesh) {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) return false;
        
        auto vertex_count = mesh.vertex_count();
        auto face_count = mesh.face_count();
        
        // Header
        file << "ply\n";
        file << "format ascii 1.0\n";
        file << "element vertex " << vertex_count << "\n";
        file << "property float x\n";
        file << "property float y\n";
        file << "property float z\n";
        file << "element face " << face_count << "\n";
        file << "property list uchar int vertex_indices\n";
        file << "end_header\n";
        
        // Vertices
        const auto& vertices = mesh.vertices();
        for (std::size_t i = 0; i < vertex_count; ++i) {
            const auto& pos = vertices[i].position;
            file << pos.x << " " << pos.y << " " << pos.z << "\n";
        }
        
        // Faces 
        const auto& faces = mesh.faces();
        for (std::size_t i = 0; i < face_count; ++i) {
            const auto& face = faces[i];
            file << face.vertices.size();
            for (std::size_t j = 0; j < face.vertices.size(); ++j) {
                file << " " << face.vertices[j];
            }
            file << "\n";
        }
        
        return true;
        
    } catch (...) {
        return false;
    }
}

int main() {
    std::cout << "Working PLY test..." << std::endl;
    
    try {
        // Create mesh
        core::Mesh<float> mesh;
        
        // Add vertices
        auto v1 = mesh.add_vertex(core::Vertex<float>(math::Vector3<float>(0.0f, 0.0f, 0.0f)));
        auto v2 = mesh.add_vertex(core::Vertex<float>(math::Vector3<float>(1.0f, 0.0f, 0.0f)));
        auto v3 = mesh.add_vertex(core::Vertex<float>(math::Vector3<float>(0.5f, 1.0f, 0.0f)));
        
        // Add triangle
        mesh.add_triangle(v1, v2, v3);
        
        std::cout << "Created mesh with " << mesh.vertex_count() << " vertices and " << mesh.face_count() << " faces" << std::endl;
        
        // Save PLY
        if (safe_ply_save("working_triangle.ply", mesh)) {
            std::cout << "PLY file saved successfully!" << std::endl;
        } else {
            std::cout << "Failed to save PLY file" << std::endl;
        }
        
        std::cout << "Working PLY test completed!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}