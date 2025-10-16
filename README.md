# Metal 4 C++ Example

This project is a minimal C++ 23 example for using **Metal 4** with GLFW in a CMake project.

### Dependencies

Dependencies are expected to be found under the `external` directory.

1. The [fetchMetalCpp](cmake/fetchMetalCpp.cmake) script downloads, extracts metal-cpp version from thhe [Apple Developer website](https://developer.apple.com/metal/cpp/), and runs the metal-cpp python script to generate the single `<metal/metal.hpp>` header file.

2. [GLFW](https://github.com/glfw/glfw) is downloaded as a git submodule.
