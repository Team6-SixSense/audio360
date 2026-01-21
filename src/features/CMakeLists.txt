# src/features CMakeLists.txt

# Add subdirectories (each adds sources/includes).
add_subdirectory(classification)
add_subdirectory(doa)
add_subdirectory(signal_processing)

# Add include directories.
target_include_directories(${SourceLib} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
