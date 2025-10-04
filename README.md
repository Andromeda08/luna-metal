# Metal 4 C++ Example

This project is a minimal C++ 23 example for using **Metal 4** with GLFW in a CMake project.

## Setup
❗As of September 2025 `metal-cpp` for macOS 26 is in beta (2) which can be downloaded from the [Apple Developer website](https://developer.apple.com/metal/cpp/).

### Dependencies

Dependencies should be extracted and placed under the `external` directory.

1. The [fetchMetalCpp](cmake/fetchMetalCpp.cmake) script downloads, extracts the used metal-cpp version and runs the python command to generate the single `<metal/metal.hpp>` header file.

2. [GLFW](https://github.com/glfw/glfw) is downloaded as a git submodule.
