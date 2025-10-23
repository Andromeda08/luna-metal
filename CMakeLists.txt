cmake_minimum_required(VERSION 4.0)
project(LunaMetal CXX OBJCXX)

enable_language(OBJCXX)

set(CMAKE_OSX_ARCHITECTURES "arm64")
set(CMAKE_CXX_STANDARD 23)

include(cmake/fetchMetalCpp.cmake)

add_subdirectory(external/glfw)

add_executable(LunaMetal
    src/main.cpp
        src/MetalRHI/metal_cpp_impl.cpp
        src/MetalRHI/MetalRHI.hpp
        src/MetalRHI/MetalRHI.cpp
        src/MetalRHI/IWindow.h
        src/Window/Window.h
        src/Window/Window.cpp
        src/Window/Window.mm
)

target_link_libraries(LunaMetal PUBLIC
    glfw
    "-framework Metal"
    "-framework Foundation"
    "-framework QuartzCore"
)
target_include_directories(LunaMetal PUBLIC
    ./src
    ${PROJECT_SOURCE_DIR}/external/glfw/include
    ${PROJECT_SOURCE_DIR}/external/metal-cpp/SingleHeader
)
