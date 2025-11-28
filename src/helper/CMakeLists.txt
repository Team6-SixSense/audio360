# src/helper CMakeLists.txt

# Add subdirectories (each adds sources/includes).
add_subdirectory(logging)

if(NOT ARM_BUILD)
    add_subdirectory(mp3)
endif()

# Add include directories.
target_include_directories(${SourceLib} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
