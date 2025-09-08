#include <iostream>
#include <cassert>
#include <cmath>
#include "include/polygon_mesh/polygon_mesh.hpp"

using namespace polygon_mesh;
using std::cout, std::endl;

int main() {
    cout << "=== Polygon Mesh Library Runtime Test ===" << endl;
    
    bool all_tests_passed = true;
    
    try {
        // Test 1: Vector3 operations
        cout << "\n[Test 1] Vector3 Operations..." << endl;
        {
            math::Vector3f v1(1.0f, 2.0f, 3.0f);
            math::Vector3f v2(4.0f, 5.0f, 6.0f);
            
            auto sum = v1 + v2;
            assert(sum.x == 5.0f && sum.y == 7.0f && sum.z == 9.0f);
            
            auto dot = v1.dot(v2);
            assert(dot == 32.0f); // 1*4 + 2*5 + 3*6 = 32
            
            auto cross = v1.cross(v2);
            // (2*6-3*5, 3*4-1*6, 1*5-2*4) = (-3, 6, -3)
            assert(cross.x == -3.0f && cross.y == 6.0f && cross.z == -3.0f);
            
            cout << "  ✓ Vector operations working correctly" << endl;
        }
        
        // Test 2: Mesh creation and basic operations
        cout << "\n[Test 2] Mesh Creation..." << endl;
        {
            core::Meshf mesh;
            
            // Add triangle vertices
            auto v1 = mesh.add_vertex({0.0f, 0.0f, 0.0f});
            auto v2 = mesh.add_vertex({1.0f, 0.0f, 0.0f});
            auto v3 = mesh.add_vertex({0.5f, 1.0f, 0.0f});
            
            assert(mesh.vertex_count() == 3);
            assert(v1 == 0 && v2 == 1 && v3 == 2);
            
            // Add triangle face
            auto face_id = mesh.add_triangle(v1, v2, v3);
            assert(mesh.face_count() == 1);
            assert(face_id == 0);
            
            cout << "  ✓ Mesh creation working correctly" << endl;
        }
        
        // Test 3: Normal computation
        cout << "\n[Test 3] Normal Computation..." << endl;
        {
            core::Meshf mesh;
            
            // Create a simple triangle in XY plane
            mesh.add_vertex({0.0f, 0.0f, 0.0f});
            mesh.add_vertex({1.0f, 0.0f, 0.0f});
            mesh.add_vertex({0.0f, 1.0f, 0.0f});
            mesh.add_triangle(0, 1, 2);
            
            mesh.compute_normals();
            
            const auto& face = mesh.get_face(0);
            // Normal should point in +Z direction for counter-clockwise vertices
            assert(face.normal.z > 0.9f); // Should be approximately (0,0,1)
            
            cout << "  ✓ Normal computation working correctly" << endl;
            cout << "    Face normal: (" << face.normal.x << ", " 
                 << face.normal.y << ", " << face.normal.z << ")" << endl;
        }
        
        // Test 4: Bounding box computation
        cout << "\n[Test 4] Bounding Box..." << endl;
        {
            core::Meshf mesh;
            
            mesh.add_vertex({-1.0f, -2.0f, -3.0f});
            mesh.add_vertex({4.0f, 5.0f, 6.0f});
            mesh.add_vertex({2.0f, 1.0f, 0.0f});
            
            const auto& bbox = mesh.bounding_box();
            
            assert(bbox.min_point.x == -1.0f);
            assert(bbox.min_point.y == -2.0f); 
            assert(bbox.min_point.z == -3.0f);
            assert(bbox.max_point.x == 4.0f);
            assert(bbox.max_point.y == 5.0f);
            assert(bbox.max_point.z == 6.0f);
            
            auto center = bbox.center();
            assert(center.x == 1.5f && center.y == 1.5f && center.z == 1.5f);
            
            cout << "  ✓ Bounding box computation working correctly" << endl;
            cout << "    Min: (" << bbox.min_point.x << ", " << bbox.min_point.y << ", " << bbox.min_point.z << ")" << endl;
            cout << "    Max: (" << bbox.max_point.x << ", " << bbox.max_point.y << ", " << bbox.max_point.z << ")" << endl;
        }
        
        // Test 5: Surface area calculation
        cout << "\n[Test 5] Surface Area..." << endl;
        {
            core::Meshf mesh;
            
            // Create unit triangle (base=1, height=1, area=0.5)
            mesh.add_vertex({0.0f, 0.0f, 0.0f});
            mesh.add_vertex({1.0f, 0.0f, 0.0f});
            mesh.add_vertex({0.0f, 1.0f, 0.0f});
            mesh.add_triangle(0, 1, 2);
            
            float area = mesh.surface_area();
            assert(std::abs(area - 0.5f) < 1e-6f);
            
            cout << "  ✓ Surface area calculation working correctly" << endl;
            cout << "    Triangle area: " << area << endl;
        }
        
        // Test 6: Edge topology
        cout << "\n[Test 6] Edge Topology..." << endl;
        {
            core::Meshf mesh;
            
            // Add square (2 triangles)
            mesh.add_vertex({0.0f, 0.0f, 0.0f});  // 0
            mesh.add_vertex({1.0f, 0.0f, 0.0f});  // 1  
            mesh.add_vertex({1.0f, 1.0f, 0.0f});  // 2
            mesh.add_vertex({0.0f, 1.0f, 0.0f});  // 3
            
            mesh.add_triangle(0, 1, 2);  // First triangle
            mesh.add_triangle(0, 2, 3);  // Second triangle
            
            assert(mesh.face_count() == 2);
            assert(mesh.edge_count() == 5); // 5 edges in 2 connected triangles
            
            cout << "  ✓ Edge topology working correctly" << endl;
            cout << "    Faces: " << mesh.face_count() << ", Edges: " << mesh.edge_count() << endl;
        }
        
        cout << "\n=== All Tests Passed! ===" << endl;
        cout << "✅ Library is working correctly!" << endl;
        
    } catch (const std::exception& e) {
        cout << "❌ Test failed with exception: " << e.what() << endl;
        all_tests_passed = false;
    } catch (...) {
        cout << "❌ Test failed with unknown exception" << endl;
        all_tests_passed = false;
    }
    
    return all_tests_passed ? 0 : 1;
}