# Source code CMakeLists.txt
enable_language(ASM)

set(SourceExecutable "Audio360.elf")
set(SourceLib "Audio360Lib")

if(NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(lib/fftw-3.3.10/)
endif()

# Collect source files.
file(GLOB_RECURSE SRC_FILES 
    helper/*.cpp
    signal_processing/*.cpp
)
set(LIB_SRC_FILES ${SRC_FILES})
list(FILTER LIB_SRC_FILES EXCLUDE REGEX "main.cpp")

# Create a library from source code.
add_library(${SourceLib} STATIC ${LIB_SRC_FILES})
target_sources(${SourceLib} PUBLIC ${LIB_SRC_FILES})

# Define executable.

# TODO: remove later
# if(NOT CI_RUN)
#     add_executable(${SourceExecutable} main.cpp)
# else()
#     # Build as static library for CI to skip linking steps.
#     add_library(${SourceExecutable} STATIC main.cpp)
# endif()
add_executable(${SourceExecutable} main.cpp ${EXTRA_SOURCES})

set(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

# Attach include dirs to the target.
target_include_directories(${SourceExecutable} PUBLIC ${INCLUDE_DIRS} )
target_include_directories(${SourceLib} PUBLIC ${INCLUDE_DIRS} )

# Add definitions/includes if cross-compiling (toolchain sets them)
if(DEFINED EXTRA_DEFS)
    target_compile_definitions(${SourceExecutable} PRIVATE ${EXTRA_DEFS})
endif()

if(DEFINED EXTRA_INCLUDES)
    target_include_directories(${SourceExecutable} PRIVATE ${EXTRA_INCLUDES})
endif()


if(NOT CMAKE_CROSSCOMPILING)
target_link_libraries(${SourceLib} PUBLIC
    fftw3
)
endif()

target_link_libraries(${SourceExecutable} PUBLIC
    ${SourceLib}
)

if(NOT CMAKE_CROSSCOMPILING)
add_custom_command(TARGET ${SourceExecutable} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:fftw3>        # This is the DLL built by CMake
    $<TARGET_FILE_DIR:${SourceExecutable}>  # The folder where your exe is
)
endif()