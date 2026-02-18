# test/helper/operations/angles CMakeLists.txt

# Define test executable files.
target_sources(${TestExecutable} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/angles_degToRad_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/angles_normalization_test.cpp
)

# Add include directories.
target_include_directories(${TestExecutable} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
