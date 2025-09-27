# Source code CMakeLists.txt

set(SourceExecutable "Audio360")
set(SourceLib "Audio360Lib")

add_subdirectory(lib/fftw-3.3.10/)


# Collect source files.
file(GLOB_RECURSE SRC_FILES 
    helper/*.cpp
    signal_processing/*.cpp
)
set(LIB_SRC_FILES ${SRC_FILES})
list(FILTER LIB_SRC_FILES EXCLUDE REGEX "main.cpp")

# Create a library from source code.
add_library(${SourceLib} STATIC ${LIB_SRC_FILES})

# Define executable.
if(NOT CI_RUN)
    add_executable(${SourceExecutable} main.cpp)
else()
    # Build as static library for CI to skip linking steps.
    add_library(${SourceExecutable} STATIC main.cpp)
endif()

# Attach include dirs to the target.
target_include_directories(${SourceExecutable} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${SourceLib} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${SourceLib} PRIVATE
    fftw3
)

target_link_libraries(${SourceExecutable} PRIVATE
    ${SourceLib}
    fftw3
)


add_custom_command(TARGET ${SourceExecutable} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:fftw3>        # This is the DLL built by CMake
    $<TARGET_FILE_DIR:${SourceExecutable}>  # The folder where your exe is
)