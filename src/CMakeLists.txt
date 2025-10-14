# Source code CMakeLists.txt
enable_language(ASM)

set(SourceExecutable "Audio360.elf")
set(SourceLib "Audio360Lib")

# Collect source files.
file(GLOB_RECURSE SRC_FILES
    helper/*.cpp
    features/*.cpp
    hardware_interface/*.*
)

set(LIB_SRC_FILES ${SRC_FILES})
list(FILTER LIB_SRC_FILES EXCLUDE REGEX "main.cpp")

add_definitions(-DDEBUG -DUSE_HAL_DRIVER -DSTM32F767xx)

# Create a library from source code.
add_library(${SourceLib} STATIC ${LIB_SRC_FILES} )
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

target_include_directories(${SourceExecutable} PUBLIC ${INCLUDE_DIRS} )
target_include_directories(${SourceLib} PUBLIC ${INCLUDE_DIRS} )

if(DEFINED EXTRA_INCLUDES)
    target_include_directories(${SourceExecutable} PRIVATE ${EXTRA_INCLUDES})
    target_include_directories(${SourceLib} PRIVATE ${EXTRA_INCLUDES})
endif()

if(NOT CMAKE_CROSSCOMPILING)
    # Attach include dirs to the target.
    set(NON_PROD_LIB_INCLUDE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/simple_fft/include/simple_fft
    )

    target_include_directories(${SourceExecutable} PRIVATE ${NON_PROD_LIB_INCLUDE_DIR})
    target_include_directories(${SourceLib} PRIVATE ${NON_PROD_LIB_INCLUDE_DIR})
endif()


target_link_libraries(${SourceExecutable} PUBLIC
    ${SourceLib}
)
