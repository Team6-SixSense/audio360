# src/hardware_interface/system CMakeLists.txt

# Add individual source files.
# Below need to link to executable to avoid watchdog interupt signal.
target_sources(${SourceExecutable} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/peripheral.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32f7xx_hal_msp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32f7xx_it.c
    ${CMAKE_CURRENT_SOURCE_DIR}/syscall.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sysmem.c
)

# Add include directories.
target_include_directories(${SourceLib} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
