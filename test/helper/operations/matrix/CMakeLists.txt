# test/helper/operations/matrix CMakeLists.txt

# Define test executable files.
target_sources(${TestExecutable} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/matrix_addition_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/matrix_creation_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/matrix_multiplication_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/matrix_scale_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/matrix_subtraction_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/matrix_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/matrix_transpose_test.cpp
)

# Add include directories.
target_include_directories(${TestExecutable} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
