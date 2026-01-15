# src/features/signal_processing CMakeLists.txt

# Add individual source files.
target_sources(${SourceLib} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/fft.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ifft.cpp
)

# Add include directories.
target_include_directories(${SourceLib} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
