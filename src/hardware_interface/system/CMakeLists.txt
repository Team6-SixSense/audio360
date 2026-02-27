# src/hardware_interface/system CMakeLists.txt

# Add individual C source files.
# Below need to link to executable to avoid watchdog interupt signal.
if(ARM_BUILD)
    target_sources(${SourceExecutable} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/peripheral.c
        ${CMAKE_CURRENT_SOURCE_DIR}/stm32f7xx_hal_msp.c
        ${CMAKE_CURRENT_SOURCE_DIR}/stm32f7xx_it.c
        ${CMAKE_CURRENT_SOURCE_DIR}/syscall.c
        ${CMAKE_CURRENT_SOURCE_DIR}/sysmem.c
    )
endif()

target_sources(${SourceLib} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/peripheral_error.cpp
)

# Add include directories.
target_include_directories(${SourceLib} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
