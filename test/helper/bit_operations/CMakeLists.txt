# test/helper/bit_operations CMakeLists.txt

# Define test executable files.
target_sources(${TestExecutable} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/sign_extend_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/swap_byte_order_test.cpp
)

# Add include directories.
target_include_directories(${TestExecutable} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
