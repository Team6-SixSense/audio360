# src/features/classification CMakeLists.txt

add_subdirectory(mel_filter)
add_subdirectory(discrete_cosine_transform)
add_subdirectory(principle_component_analysis)
add_subdirectory(linear_discriminant_analysis)

# Add include directories.
target_include_directories(${SourceLib} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
