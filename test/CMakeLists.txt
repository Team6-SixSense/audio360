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
FetchContent_MakeAvailable(googletest)

# Collect test source files
file(GLOB TEST_SOURCES *.cpp)

# Create test executable
add_executable(${TestExecutable} ${TEST_SOURCES})

# Link test executable against your library + gtest
target_link_libraries(${TestExecutable}
    PRIVATE
    Audio360Lib
    GTest::gtest_main
)

# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(${TestExecutable})
