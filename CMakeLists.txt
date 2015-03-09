#------------------------------------------------------------------------------
# OpenGL Tutorials
#------------------------------------------------------------------------------
project(nxgltutorials)
cmake_minimum_required(VERSION 2.8)

include(CMakeParseArguments)

if (NOT NX_SDK_DIR)
    message(FATAL_ERROR "Please define NX_SDK_DIR!")
endif()

set(CMAKE_MODULE_PATH ${NX_SDK_DIR}/cmake)

include(nxarchive)
include(nxresources)
include(nxcrosscompile)
include(nxapp)

include(${NX_SDK_DIR}/${NX_HOST_PREFIX}/cmake-export/nx_export.cmake)
include(${NX_SDK_DIR}/${NX_HOST_PREFIX}/cmake-export/nxconfig.cmake)

set(CMAKE_C_FLAGS ${NX_C_FLAGS})
set(CMAKE_CXX_FLAGS ${NX_CXX_FLAGS})
set(CMAKE_C_FLAGS_RELEASE ${NX_C_FLAGS_RELEASE})
set(CMAKE_CXX_FLAGS_RELEASE ${NX_CXX_FLAGS_RELEASE})
set(CMAKE_C_FLAGS_DEBUG ${NX_C_FLAGS_DEBUG})
set(CMAKE_CXX_FLAGS_DEBUG ${NX_CXX_FLAGS_DEBUG})



include_directories(/usr/local/include)
include_directories(${NX_SDK_DIR}/${NX_HOST_PREFIX}/include)
link_directories(${NX_SDK_DIR}/${NX_HOST_PREFIX}/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common)
add_subdirectory(common)
add_subdirectory(camera)
add_subdirectory(triangle)
add_subdirectory(fonts)
add_subdirectory(shadows)
