set(DEPENDENCIES_DIR ${PROJECT_SOURCE_DIR}/external)
set(METAL_CPP_DIR ${PROJECT_SOURCE_DIR}/external/metal-cpp)

if (NOT EXISTS ${METAL_CPP_DIR})
    file(
        DOWNLOAD
        https://developer.apple.com/metal/cpp/files/metal-cpp_macOS26_iOS26-beta2.zip
        ${CMAKE_CURRENT_BINARY_DIR}/metal-cpp.zip
        SHOW_PROGRESS
        STATUS DOWNLOAD_STATUS
    )

    list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
    if(NOT STATUS_CODE EQUAL 0)
        message(FATAL_ERROR "Failed to download metal-cpp")
    endif()

    file(ARCHIVE_EXTRACT
        INPUT ${CMAKE_CURRENT_BINARY_DIR}/metal-cpp.zip
        DESTINATION ${DEPENDENCIES_DIR}
    )

    find_package(Python3 COMPONENTS Interpreter REQUIRED)
    execute_process(
        COMMAND ${Python3_EXECUTABLE} ./SingleHeader/MakeSingleHeader.py -o SingleHeader/metal/metal.hpp Foundation/Foundation.hpp QuartzCore/QuartzCore.hpp Metal/Metal.hpp MetalFX/MetalFX.hpp
        WORKING_DIRECTORY ${METAL_CPP_DIR}
    )
endif ()
