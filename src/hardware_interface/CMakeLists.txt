# src/hardware_interface CMakeLists.txt

# Add subdirectories (each adds sources/includes).
add_subdirectory(system)

# Add include directories.
target_include_directories(${SourceLib} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
