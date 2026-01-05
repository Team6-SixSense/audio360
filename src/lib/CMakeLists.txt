# src/lib CMakeLists.txt

# CMSIS-DSP lib
# =============

# Add subdirectories (each adds sources/includes).
if (ARM_BUILD)
    target_sources(${SourceLib} PUBLIC
        # FFT functions.
        ${CMAKE_CURRENT_LIST_DIR}/STM32CubeF7/Drivers/CMSIS/DSP/Source/TransformFunctions/arm_bitreversal2.S
        ${CMAKE_CURRENT_LIST_DIR}/STM32CubeF7/Drivers/CMSIS/DSP/Source/TransformFunctions/arm_rfft_fast_f32.c
        ${CMAKE_CURRENT_LIST_DIR}/STM32CubeF7/Drivers/CMSIS/DSP/Source/TransformFunctions/arm_rfft_fast_init_f32.c
        ${CMAKE_CURRENT_LIST_DIR}/STM32CubeF7/Drivers/CMSIS/DSP/Source/TransformFunctions/arm_cfft_f32.c
        ${CMAKE_CURRENT_LIST_DIR}/STM32CubeF7/Drivers/CMSIS/DSP/Source/TransformFunctions/arm_cfft_radix8_f32.c
        ${CMAKE_CURRENT_LIST_DIR}/STM32CubeF7/Drivers/CMSIS/DSP/Source/TransformFunctions/arm_rfft_init_q15.c
        ${CMAKE_CURRENT_LIST_DIR}/STM32CubeF7/Drivers/CMSIS/DSP/Source/TransformFunctions/arm_rfft_init_q31.c
        ${CMAKE_CURRENT_LIST_DIR}/STM32CubeF7/Drivers/CMSIS/DSP/Source/CommonTables/arm_common_tables.c
        ${CMAKE_CURRENT_LIST_DIR}/STM32CubeF7/Drivers/CMSIS/DSP/Source/CommonTables/arm_const_structs.c
    )
else()
    target_sources(${SourceLib} PUBLIC
        # FFT functions.
        ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS-DSP/Source/TransformFunctions/arm_bitreversal2.c
        ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS-DSP/Source/TransformFunctions/arm_cfft_init_f32.c
        ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS-DSP/Source/TransformFunctions/arm_rfft_fast_f32.c
        ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS-DSP/Source/TransformFunctions/arm_rfft_fast_init_f32.c
        ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS-DSP/Source/TransformFunctions/arm_cfft_f32.c
        ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS-DSP/Source/TransformFunctions/arm_cfft_radix8_f32.c
        ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS-DSP/Source/TransformFunctions/arm_rfft_init_q15.c
        ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS-DSP/Source/TransformFunctions/arm_rfft_init_q31.c
        ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS-DSP/Source/CommonTables/arm_common_tables.c
        ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS-DSP/Source/CommonTables/arm_const_structs.c
    )
endif()

# Add include directories.
if (ARM_BUILD)
    target_include_directories(${SourceLib} PUBLIC
        # FFT functions.
        ${CMAKE_CURRENT_LIST_DIR}/STM32CubeF7/Drivers/CMSIS/Include
        ${CMAKE_CURRENT_LIST_DIR}/STM32CubeF7/Drivers/CMSIS/Core/Include
        ${CMAKE_CURRENT_LIST_DIR}/STM32CubeF7/Drivers/CMSIS/DSP/Include
        ${CMAKE_CURRENT_LIST_DIR}/STM32CubeF7/Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/inc
    )
else()
    target_include_directories(${SourceLib} PUBLIC
        # FFT functions.
        ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS-DSP/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS-DSP/PrivateInclude
    )
endif()
