# Test code CMakeLists.txt

set(TestExecutable "Audio360Tests")

# Enable testing support
enable_testing()

# Download and make GoogleTest available
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

# Prevent GoogleTest from forcing shared CRT on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Create test executable
add_executable(${TestExecutable})
add_dependencies(${TestExecutable} Audio360Lib)
add_subdirectory(features)
add_subdirectory(helper)

# Link test executable against your library + gtest
target_link_libraries(${TestExecutable} PRIVATE
    Audio360Lib
    GTest::gtest_main
    GTest::gmock_main 
)

# Link compile definitions.
target_compile_definitions(${TestExecutable} PUBLIC
    ARM_MATH_MATRIX_CHECK
)

# Register tests with CTest
include(GoogleTest)

# Store the test executable in the same location as the Audio360 executable.
set_target_properties(${TestExecutable} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/src"
)

# Copy audio test data.
add_custom_command(
    TARGET ${TestExecutable}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/audio"
        $<TARGET_FILE_DIR:${TestExecutable}>/audio
    COMMENT "Copying test data files for ${TestExecutable}"
)