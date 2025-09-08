#include <iostream>
#include <chrono>
#include <vector>
#include <random>
#include <polygon_mesh/polygon_mesh.hpp>

using namespace polygon_mesh;
using namespace polygon_mesh::core;

class Timer {
private:
    std::chrono::high_resolution_clock::time_point start_time;
    
public:
    void start() {
        start_time = std::chrono::high_resolution_clock::now();
    }
    
    double elapsed_ms() const {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        return duration.count() / 1000.0; // Convert to milliseconds
    }
};

void test_large_mesh_creation() {
    std::cout << "Testing large mesh creation performance..." << std::endl;
    
    const size_t num_vertices = 10000;
    const size_t num_faces = 19998; // Approximately 2 * vertices for a good mesh
    
    core::Meshf mesh;
    mesh.reserve_vertices(num_vertices);
    mesh.reserve_faces(num_faces);
    
    Timer timer;
    timer.start();
    
    // Generate random vertices
    std::mt19937 gen(42); // Fixed seed for reproducibility
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<VertexId> vertex_ids;
    vertex_ids.reserve(num_vertices);
    
    for (size_t i = 0; i < num_vertices; ++i) {
        math::Vector3f pos(dist(gen), dist(gen), dist(gen));
        vertex_ids.push_back(mesh.add_vertex(pos));
    }
    
    double vertex_time = timer.elapsed_ms();
    std::cout << "  Added " << num_vertices << " vertices in " << vertex_time << " ms" << std::endl;
    std::cout << "  Average: " << (vertex_time / num_vertices * 1000.0) << " μs per vertex" << std::endl;
    
    // Generate random triangular faces
    timer.start();
    
    std::uniform_int_distribution<size_t> vertex_dist(0, num_vertices - 1);
    size_t faces_added = 0;
    
    for (size_t i = 0; i < num_faces && faces_added < num_faces; ++i) {
        VertexId v1 = static_cast<VertexId>(vertex_dist(gen));
        VertexId v2 = static_cast<VertexId>(vertex_dist(gen));
        VertexId v3 = static_cast<VertexId>(vertex_dist(gen));
        
        // Avoid degenerate triangles
        if (v1 != v2 && v2 != v3 && v1 != v3) {
            try {
                mesh.add_triangle(v1, v2, v3);
                faces_added++;
            } catch (...) {
                // Skip invalid faces
            }
        }
    }
    
    double face_time = timer.elapsed_ms();
    std::cout << "  Added " << faces_added << " faces in " << face_time << " ms" << std::endl;
    std::cout << "  Average: " << (face_time / faces_added * 1000.0) << " μs per face" << std::endl;
    
    std::cout << "  Final mesh stats: " << mesh.vertex_count() << " vertices, " 
              << mesh.face_count() << " faces, " << mesh.edge_count() << " edges" << std::endl;
}

void test_normal_computation_performance() {
    std::cout << "\nTesting normal computation performance..." << std::endl;
    
    // Create a reasonably complex mesh (sphere-like)
    core::Meshf mesh;
    
    const int subdivisions = 50;
    const float radius = 10.0f;
    
    Timer timer;
    timer.start();
    
    // Generate vertices on a sphere
    std::vector<VertexId> vertex_ids;
    for (int i = 0; i <= subdivisions; ++i) {
        for (int j = 0; j <= subdivisions; ++j) {
            float theta = static_cast<float>(i) / subdivisions * math::pi<float>();
            float phi = static_cast<float>(j) / subdivisions * 2.0f * math::pi<float>();
            
            float x = radius * std::sin(theta) * std::cos(phi);
            float y = radius * std::sin(theta) * std::sin(phi);
            float z = radius * std::cos(theta);
            
            vertex_ids.push_back(mesh.add_vertex(math::Vector3f(x, y, z)));
        }
    }
    
    // Generate faces
    for (int i = 0; i < subdivisions; ++i) {
        for (int j = 0; j < subdivisions; ++j) {
            int current = i * (subdivisions + 1) + j;
            int next = current + subdivisions + 1;
            
            // First triangle
            if (i > 0) { // Skip top cap
                mesh.add_triangle(vertex_ids[current], vertex_ids[current + 1], vertex_ids[next + 1]);
            }
            
            // Second triangle
            if (i < subdivisions - 1) { // Skip bottom cap
                mesh.add_triangle(vertex_ids[current], vertex_ids[next + 1], vertex_ids[next]);
            }
        }
    }
    
    double creation_time = timer.elapsed_ms();
    std::cout << "  Created sphere mesh in " << creation_time << " ms" << std::endl;
    std::cout << "  Mesh stats: " << mesh.vertex_count() << " vertices, " << mesh.face_count() << " faces" << std::endl;
    
    // Test normal computation
    timer.start();
    mesh.compute_normals();
    double normal_time = timer.elapsed_ms();
    
    std::cout << "  Computed normals in " << normal_time << " ms" << std::endl;
    std::cout << "  Average per vertex: " << (normal_time / mesh.vertex_count() * 1000.0) << " μs" << std::endl;
}

void test_bounding_box_performance() {
    std::cout << "\nTesting bounding box computation performance..." << std::endl;
    
    const size_t num_vertices = 100000;
    core::Meshf mesh;
    
    // Generate random vertices
    std::mt19937 gen(123);
    std::uniform_real_distribution<float> dist(-1000.0f, 1000.0f);
    
    Timer timer;
    timer.start();
    
    for (size_t i = 0; i < num_vertices; ++i) {
        mesh.add_vertex(math::Vector3f(dist(gen), dist(gen), dist(gen)));
    }
    
    double creation_time = timer.elapsed_ms();
    
    // Test bounding box computation
    timer.start();
    const auto& bbox = mesh.bounding_box();
    double bbox_time = timer.elapsed_ms();
    
    std::cout << "  Created " << num_vertices << " vertices in " << creation_time << " ms" << std::endl;
    std::cout << "  Computed bounding box in " << bbox_time << " ms" << std::endl;
    std::cout << "  Bounding box: min" << bbox.min_point << " max" << bbox.max_point << std::endl;
    std::cout << "  Volume: " << bbox.volume() << std::endl;
}

void test_memory_usage() {
    std::cout << "\nTesting memory usage..." << std::endl;
    
    const size_t num_vertices = 50000;
    
    core::Meshf mesh;
    
    Timer timer;
    timer.start();
    
    // Add vertices and measure
    for (size_t i = 0; i < num_vertices; ++i) {
        float x = static_cast<float>(i % 100);
        float y = static_cast<float>((i / 100) % 100);
        float z = static_cast<float>(i / 10000);
        
        mesh.add_vertex(math::Vector3f(x, y, z));
        
        // Add some faces to create edges
        if (i >= 2) {
            if (i % 3 == 2) { // Every third vertex, create a triangle
                try {
                    mesh.add_triangle(static_cast<VertexId>(i-2), 
                                    static_cast<VertexId>(i-1), 
                                    static_cast<VertexId>(i));
                } catch (...) {
                    // Skip any invalid triangles
                }
            }
        }
    }
    
    double total_time = timer.elapsed_ms();
    
    std::cout << "  Created mesh with " << mesh.vertex_count() << " vertices, " 
              << mesh.face_count() << " faces, " << mesh.edge_count() << " edges" << std::endl;
    std::cout << "  Total time: " << total_time << " ms" << std::endl;
    std::cout << "  Average per vertex: " << (total_time / num_vertices * 1000.0) << " μs" << std::endl;
    
    // Estimate memory usage
    size_t vertex_memory = mesh.vertex_count() * sizeof(core::Vertexf);
    size_t face_memory = mesh.face_count() * sizeof(core::Facef);
    size_t edge_memory = mesh.edge_count() * sizeof(core::Edgef);
    size_t total_memory = vertex_memory + face_memory + edge_memory;
    
    std::cout << "  Estimated memory usage: " << (total_memory / 1024.0 / 1024.0) << " MB" << std::endl;
    std::cout << "  Memory per vertex: " << (total_memory / mesh.vertex_count()) << " bytes" << std::endl;
}

int main() {
    std::cout << "=== Polygon Mesh Library - Performance Test Suite ===" << std::endl;
    
    try {
        test_large_mesh_creation();
        test_normal_computation_performance();
        test_bounding_box_performance();
        test_memory_usage();
        
        std::cout << "\n=== All performance tests completed! ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Performance test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}