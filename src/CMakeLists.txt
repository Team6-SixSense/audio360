# Source code CMakeLists.txt
enable_language(ASM)

set(SourceExecutable "Audio360.elf")
set(SourceLib "Audio360Lib")

# Collect source files.
file(GLOB_RECURSE SRC_FILES
    helper/*.cpp
    features/*.cpp
)

set(LIB_SRC_FILES ${SRC_FILES})
list(FILTER LIB_SRC_FILES EXCLUDE REGEX "main.cpp")

if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
    set(CMSIS_DSP_NON_ARM_SRC_FILES
        # FFT functions.
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/CMSIS-DSP/Source/TransformFunctions/arm_bitreversal2.c
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/CMSIS-DSP/Source/TransformFunctions/arm_cfft_init_f32.c
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/CMSIS-DSP/Source/TransformFunctions/arm_rfft_fast_f32.c
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/CMSIS-DSP/Source/TransformFunctions/arm_rfft_fast_init_f32.c
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/CMSIS-DSP/Source/TransformFunctions/arm_cfft_f32.c
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/CMSIS-DSP/Source/TransformFunctions/arm_cfft_radix8_f32.c
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/CMSIS-DSP/Source/TransformFunctions/arm_rfft_init_q15.c
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/CMSIS-DSP/Source/TransformFunctions/arm_rfft_init_q31.c
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/CMSIS-DSP/Source/CommonTables/arm_common_tables.c
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/CMSIS-DSP/Source/CommonTables/arm_const_structs.c
    )
    
    list(APPEND LIB_SRC_FILES ${CMSIS_DSP_NON_ARM_SRC_FILES})
endif()

# Create a library from source code.
add_library(${SourceLib} STATIC ${LIB_SRC_FILES})
target_sources(${SourceLib} PUBLIC ${LIB_SRC_FILES})

# Define executable.
add_executable(${SourceExecutable}
    main.cpp
    ${EXTRA_SOURCES}
)

# Add definitions/includes if cross-compiling (toolchain sets them)
if(DEFINED EXTRA_DEFS)
    target_compile_definitions(${SourceExecutable} PRIVATE ${EXTRA_DEFS})
    target_compile_definitions(${SourceLib} PRIVATE ${EXTRA_DEFS})
endif()
target_compile_definitions(${SourceExecutable} PRIVATE LOGGING_ENABLED=$<BOOL:${LOGGING_ENABLED}>)
target_compile_definitions(${SourceLib} PRIVATE LOGGING_ENABLED=$<BOOL:${LOGGING_ENABLED}>)

# Attach include dirs to the target.
set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/configs
)

if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
    set(CMSIS_DSP_NON_ARM_INCLUDE_DIR
        # FFT functions.
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/CMSIS-DSP/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/CMSIS-DSP/PrivateInclude
    )
    
    list(APPEND INCLUDE_DIRS ${CMSIS_DSP_NON_ARM_INCLUDE_DIR})
endif()

target_include_directories(${SourceExecutable} PUBLIC ${INCLUDE_DIRS} )
target_include_directories(${SourceLib} PUBLIC ${INCLUDE_DIRS} )

if(DEFINED EXTRA_INCLUDES)
    target_include_directories(${SourceExecutable} PRIVATE ${EXTRA_INCLUDES})
    target_include_directories(${SourceLib} PRIVATE ${EXTRA_INCLUDES})
endif()

target_link_libraries(${SourceExecutable} PUBLIC
    ${SourceLib}
)
