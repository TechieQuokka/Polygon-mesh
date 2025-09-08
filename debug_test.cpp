#include <polygon_mesh/polygon_mesh.hpp>
#include <iostream>

using namespace polygon_mesh;

int main() {
    std::cout << "Starting debug test..." << std::endl;
    
    try {
        // Test basic mesh creation
        std::cout << "Creating mesh..." << std::endl;
        Mesh mesh;
        
        // Add a simple vertex
        std::cout << "Adding vertex..." << std::endl;
        mesh.add_vertex(Vector3(0.0f, 0.0f, 0.0f));
        std::cout << "Vertex added successfully" << std::endl;
        
        // Test PLY file creation first
        std::cout << "Testing PLY save..." << std::endl;
        if (io::save_ply("debug_test.ply", mesh)) {
            std::cout << "PLY save successful" << std::endl;
        } else {
            std::cout << "PLY save failed" << std::endl;
        }
        
        // Test PLY file loading
        std::cout << "Testing PLY load..." << std::endl;
        auto loaded_mesh = io::load_ply<float>("debug_test.ply");
        std::cout << "PLY load successful: " << loaded_mesh.vertex_count() << " vertices" << std::endl;
        
        std::cout << "Debug test completed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}