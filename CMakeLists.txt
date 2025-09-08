cmake_minimum_required(VERSION 3.16)
project(PolygonMesh VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler-specific options
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Options
option(POLYGON_MESH_BUILD_TESTS "Build tests" ON)
option(POLYGON_MESH_BUILD_EXAMPLES "Build examples" ON)
option(POLYGON_MESH_USE_OPENMP "Enable OpenMP support" ON)

# Find packages
if(POLYGON_MESH_USE_OPENMP)
    find_package(OpenMP)
    if(OpenMP_CXX_FOUND)
        message(STATUS "OpenMP found")
    endif()
endif()

# Header-only library
add_library(polygon_mesh INTERFACE)
add_library(polygon_mesh::polygon_mesh ALIAS polygon_mesh)

target_include_directories(polygon_mesh
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(polygon_mesh 
    INTERFACE 
        cxx_std_17
)

# Link OpenMP if available
if(OpenMP_CXX_FOUND AND POLYGON_MESH_USE_OPENMP)
    target_link_libraries(polygon_mesh 
        INTERFACE 
            OpenMP::OpenMP_CXX
    )
    target_compile_definitions(polygon_mesh 
        INTERFACE 
            POLYGON_MESH_USE_OPENMP
    )
endif()

# Examples
if(POLYGON_MESH_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Tests
if(POLYGON_MESH_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Install
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS polygon_mesh
    EXPORT polygon_mesh-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT polygon_mesh-targets
    FILE polygon_mesh-targets.cmake
    NAMESPACE polygon_mesh::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/polygon_mesh
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/polygon_mesh-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/polygon_mesh-config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/polygon_mesh
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/polygon_mesh-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/polygon_mesh-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/polygon_mesh-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/polygon_mesh
)

# Export targets for build tree
export(EXPORT polygon_mesh-targets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/polygon_mesh-targets.cmake"
    NAMESPACE polygon_mesh::
)

# Package config
set(CPACK_PACKAGE_NAME "PolygonMesh")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "High-performance C++ Polygon Mesh processing library")
include(CPack)