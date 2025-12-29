# src/helper/mp3 CMakeLists.txt

# Add individual source files.
target_sources(${SourceLib} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/mp3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/realTimePolling.cpp
)

# Add include directories.
target_include_directories(${SourceLib} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
