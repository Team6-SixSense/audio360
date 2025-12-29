# src/helper/logging CMakeLists.txt

# Add individual source files.
if (ARM_BUILD)
    target_sources(${SourceLib} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/sd_writer.cpp
    )
endif()

# Add include directories.
target_include_directories(${SourceLib} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
