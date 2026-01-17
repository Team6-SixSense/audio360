# src/runtimes CMakeLists.txt

if(ARM_BUILD)
    target_sources(${SourceLib} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/runtime_audio360.cpp
    )
endif()

# Add include directories.
target_include_directories(${SourceLib} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
