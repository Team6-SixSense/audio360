# test/helper CMakeLists.txt

# Add subdirectories (each adds sources/includes).
add_subdirectory(matrix)
add_subdirectory(mp3)

# Define test executable files.
target_sources(${TestExecutable} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/test_helper.cpp
)

# Add include directories.
target_include_directories(${TestExecutable} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
