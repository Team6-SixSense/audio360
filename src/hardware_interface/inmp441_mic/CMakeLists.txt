target_sources(${SourceExecutable} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/embedded_mic.cpp
)

# Add current directory as include directory.
target_include_directories(${SourceLib} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
