# src/hardware_interface CMakeLists.txt

# Add subdirectories (each adds sources/includes).
if(ARM_BUILD)
    add_subdirectory(system)
    add_subdirectory(inmp441_mic)
endif()

# Add include directories.
target_include_directories(${SourceLib} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
