# src/features/doa CMakeLists.txt

# Add individual source files.
target_sources(${SourceLib} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/doa.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gccPhat.cpp
)

# Add include directories.
target_include_directories(${SourceLib} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
