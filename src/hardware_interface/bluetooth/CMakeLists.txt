# src/hardware_interface/bluetooth CMakeLists.txt

target_sources(${SourceExecutable} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/bluetooth_manager.cpp
)

# Add current directory as include directory.
target_include_directories(${SourceLib} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
