# Metal 4 C++ Example

This project is a minimal C++ 23 example for using **Metal 4** with GLFW in a CMake project.

## Setup
❗As of September 2025 `metal-cpp` for macOS 26 is in beta (2) which can be downloaded from the [Apple Developer website](https://developer.apple.com/metal/cpp/).

### Dependencies

Dependencies should be extracted and placed under the `external` directory.

1. Download `metal-cpp_macOS26_iOS26-beta2` from the [Apple Developer website](https://developer.apple.com/metal/cpp/files/metal-cpp_macOS26_iOS26-beta2.zip).
   - Generate the `<metal/metal.hpp>` single header using the Python script.
    ```shell
    py ./SingleHeader/MakeSingleHeader.py -o SingleHeader/metal/metal.hpp Foundation/Foundation.hpp QuartzCore/QuartzCore.hpp Metal/Metal.hpp MetalFX/MetalFX.hpp
    ```

2. Download the lastest version of `GLFW` from [GitHub](https://github.com/glfw/glfw).
