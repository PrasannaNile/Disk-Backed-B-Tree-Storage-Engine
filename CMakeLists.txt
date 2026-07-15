cmake_minimum_required(VERSION 3.15) # latest version
project(B+TreeStorageEngine VERSION 1.0) # name of the project with version 

set(CMAKE_CXX_STANDARD 17) # force c++ 17
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# Build Artifact Routing (.obj goes to build/, .exe goes to bin/)
# ---------------------------------------------------------------------------
# This automatically routes your final executable straight into your /bin folder
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# ---------------------------------------------------------------------------
# Include Directories
# ---------------------------------------------------------------------------
# This lets you write clean includes like #include "storage/disk_manager.hpp"
include_directories(${CMAKE_SOURCE_DIR}/include)

add_compile_definitions(CONCPP_BUILD_SHARED) # defines a global preprocessor macro


add_executable(BPlusTreeStorageEngine
    src/main.cpp
    src/storage/page.cpp
    src/storage/disk_manager.cpp
    src/storage/buffer_pool_manager.cpp
    src/storage/lru_replacer.cpp

)
