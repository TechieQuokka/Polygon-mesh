#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <polygon_mesh/polygon_mesh.hpp>

using namespace polygon_mesh;
using namespace polygon_mesh::core;

// Generate a sphere mesh using UV sphere algorithm
core::Meshf create_sphere_mesh(float radius, int longitude_segments, int latitude_segments) {
    core::Meshf mesh;
    
    std::cout << "Creating sphere with radius " << radius 
              << ", longitude segments: " << longitude_segments
              << ", latitude segments: " << latitude_segments << std::endl;
    
    std::vector<VertexId> vertex_ids;
    
    // Generate vertices
    for (int lat = 0; lat <= latitude_segments; ++lat) {
        for (int lon = 0; lon <= longitude_segments; ++lon) {
            float theta = static_cast<float>(lat) / latitude_segments * math::pi<float>();
            float phi = static_cast<float>(lon) / longitude_segments * 2.0f * math::pi<float>();
            
            float x = radius * std::sin(theta) * std::cos(phi);
            float y = radius * std::cos(theta);
            float z = radius * std::sin(theta) * std::sin(phi);
            
            // Normal for sphere is just normalized position
            math::Vector3f position(x, y, z);
            math::Vector3f normal = position.normalize();
            
            // UV coordinates
            float u = static_cast<float>(lon) / longitude_segments;
            float v = static_cast<float>(lat) / latitude_segments;
            math::Vector2f uv(u, v);
            
            vertex_ids.push_back(mesh.add_vertex(position, normal, uv));
        }
    }
    
    // Generate faces
    for (int lat = 0; lat < latitude_segments; ++lat) {
        for (int lon = 0; lon < longitude_segments; ++lon) {
            int current = lat * (longitude_segments + 1) + lon;
            int next = current + longitude_segments + 1;
            
            // Skip degenerate triangles at poles
            if (lat != 0) {
                mesh.add_triangle(vertex_ids[current], 
                                vertex_ids[current + 1], 
                                vertex_ids[next + 1]);
            }
            
            if (lat != latitude_segments - 1) {
                mesh.add_triangle(vertex_ids[current], 
                                vertex_ids[next + 1], 
                                vertex_ids[next]);
            }
        }
    }
    
    return mesh;
}

// Generate a torus mesh
core::Meshf create_torus_mesh(float major_radius, float minor_radius, int major_segments, int minor_segments) {
    core::Meshf mesh;
    
    std::cout << "Creating torus with major radius " << major_radius 
              << ", minor radius " << minor_radius
              << ", major segments: " << major_segments
              << ", minor segments: " << minor_segments << std::endl;
    
    std::vector<VertexId> vertex_ids;
    
    // Generate vertices
    for (int i = 0; i <= major_segments; ++i) {
        for (int j = 0; j <= minor_segments; ++j) {
            float u = static_cast<float>(i) / major_segments * 2.0f * math::pi<float>();
            float v = static_cast<float>(j) / minor_segments * 2.0f * math::pi<float>();
            
            float x = (major_radius + minor_radius * std::cos(v)) * std::cos(u);
            float y = minor_radius * std::sin(v);
            float z = (major_radius + minor_radius * std::cos(v)) * std::sin(u);
            
            // Normal calculation for torus
            float nx = std::cos(v) * std::cos(u);
            float ny = std::sin(v);
            float nz = std::cos(v) * std::sin(u);
            
            math::Vector3f position(x, y, z);
            math::Vector3f normal(nx, ny, nz);
            
            float tex_u = static_cast<float>(i) / major_segments;
            float tex_v = static_cast<float>(j) / minor_segments;
            math::Vector2f uv(tex_u, tex_v);
            
            vertex_ids.push_back(mesh.add_vertex(position, normal, uv));
        }
    }
    
    // Generate faces
    for (int i = 0; i < major_segments; ++i) {
        for (int j = 0; j < minor_segments; ++j) {
            int current = i * (minor_segments + 1) + j;
            int next = ((i + 1) % (major_segments + 1)) * (minor_segments + 1) + j;
            
            mesh.add_triangle(vertex_ids[current], 
                            vertex_ids[current + 1], 
                            vertex_ids[next + 1]);
                            
            mesh.add_triangle(vertex_ids[current], 
                            vertex_ids[next + 1], 
                            vertex_ids[next]);
        }
    }
    
    return mesh;
}

// Generate a plane mesh
core::Meshf create_plane_mesh(float width, float height, int width_segments, int height_segments) {
    core::Meshf mesh;
    
    std::cout << "Creating plane with width " << width 
              << ", height " << height
              << ", width segments: " << width_segments
              << ", height segments: " << height_segments << std::endl;
    
    std::vector<VertexId> vertex_ids;
    
    // Generate vertices
    for (int y = 0; y <= height_segments; ++y) {
        for (int x = 0; x <= width_segments; ++x) {
            float px = (static_cast<float>(x) / width_segments - 0.5f) * width;
            float py = 0.0f;
            float pz = (static_cast<float>(y) / height_segments - 0.5f) * height;
            
            float u = static_cast<float>(x) / width_segments;
            float v = static_cast<float>(y) / height_segments;
            
            math::Vector3f position(px, py, pz);
            math::Vector3f normal(0.0f, 1.0f, 0.0f);
            math::Vector2f uv(u, v);
            
            vertex_ids.push_back(mesh.add_vertex(position, normal, uv));
        }
    }
    
    // Generate faces
    for (int y = 0; y < height_segments; ++y) {
        for (int x = 0; x < width_segments; ++x) {
            int current = y * (width_segments + 1) + x;
            int next = current + width_segments + 1;
            
            mesh.add_triangle(vertex_ids[current], 
                            vertex_ids[current + 1], 
                            vertex_ids[next + 1]);
                            
            mesh.add_triangle(vertex_ids[current], 
                            vertex_ids[next + 1], 
                            vertex_ids[next]);
        }
    }
    
    return mesh;
}

// Save mesh as OBJ file (simple version)
void save_mesh_as_obj(const core::Meshf& mesh, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }
    
    file << "# Generated by Polygon Mesh Library\n";
    file << "# Vertices: " << mesh.vertex_count() << "\n";
    file << "# Faces: " << mesh.face_count() << "\n\n";
    
    // Write vertices
    for (const auto& vertex : mesh.vertices()) {
        file << "v " << vertex.position.x << " " 
             << vertex.position.y << " " 
             << vertex.position.z << "\n";
    }
    
    // Write normals
    file << "\n";
    for (const auto& vertex : mesh.vertices()) {
        if (vertex.has_normal()) {
            file << "vn " << vertex.normal.x << " " 
                 << vertex.normal.y << " " 
                 << vertex.normal.z << "\n";
        }
    }
    
    // Write texture coordinates
    file << "\n";
    for (const auto& vertex : mesh.vertices()) {
        if (vertex.has_uv()) {
            file << "vt " << vertex.uv.x << " " << vertex.uv.y << "\n";
        }
    }
    
    // Write faces (OBJ uses 1-based indexing)
    file << "\n";
    for (const auto& face : mesh.faces()) {
        file << "f ";
        for (VertexId vid : face.vertices) {
            file << (vid + 1);
            if (mesh.vertices()[vid].has_uv() && mesh.vertices()[vid].has_normal()) {
                file << "/" << (vid + 1) << "/" << (vid + 1);
            } else if (mesh.vertices()[vid].has_normal()) {
                file << "//" << (vid + 1);
            }
            file << " ";
        }
        file << "\n";
    }
    
    file.close();
    std::cout << "Mesh saved to: " << filename << std::endl;
}

int main() {
    std::cout << "=== Advanced Mesh Generation Example ===" << std::endl;
    
    try {
        // Create different types of meshes
        auto sphere = create_sphere_mesh(2.0f, 20, 10);
        sphere.compute_normals();
        
        std::cout << "\nSphere Statistics:" << std::endl;
        std::cout << "  Vertices: " << sphere.vertex_count() << std::endl;
        std::cout << "  Faces: " << sphere.face_count() << std::endl;
        std::cout << "  Edges: " << sphere.edge_count() << std::endl;
        std::cout << "  Surface Area: " << sphere.surface_area() << std::endl;
        std::cout << "  Volume: " << sphere.volume() << std::endl;
        std::cout << "  Topology Valid: " << (sphere.validate_topology() ? "Yes" : "No") << std::endl;
        
        auto torus = create_torus_mesh(3.0f, 1.0f, 20, 12);
        torus.compute_normals();
        
        std::cout << "\nTorus Statistics:" << std::endl;
        std::cout << "  Vertices: " << torus.vertex_count() << std::endl;
        std::cout << "  Faces: " << torus.face_count() << std::endl;
        std::cout << "  Edges: " << torus.edge_count() << std::endl;
        std::cout << "  Surface Area: " << torus.surface_area() << std::endl;
        std::cout << "  Volume: " << torus.volume() << std::endl;
        std::cout << "  Topology Valid: " << (torus.validate_topology() ? "Yes" : "No") << std::endl;
        
        auto plane = create_plane_mesh(10.0f, 10.0f, 10, 10);
        plane.compute_normals();
        
        std::cout << "\nPlane Statistics:" << std::endl;
        std::cout << "  Vertices: " << plane.vertex_count() << std::endl;
        std::cout << "  Faces: " << plane.face_count() << std::endl;
        std::cout << "  Edges: " << plane.edge_count() << std::endl;
        std::cout << "  Surface Area: " << plane.surface_area() << std::endl;
        std::cout << "  Topology Valid: " << (plane.validate_topology() ? "Yes" : "No") << std::endl;
        
        // Save meshes to OBJ files
        save_mesh_as_obj(sphere, "sphere.obj");
        save_mesh_as_obj(torus, "torus.obj");
        save_mesh_as_obj(plane, "plane.obj");
        
        std::cout << "\n=== Advanced mesh generation completed successfully! ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}