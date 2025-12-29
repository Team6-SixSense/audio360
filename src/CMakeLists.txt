# Source code CMakeLists.txt
enable_language(ASM)

set(SourceExecutable "Audio360.elf")
set(SourceLib "Audio360Lib")

# Define executable.
add_library(${SourceLib} STATIC)

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

# Add subdirectories (each adds sources/includes).
add_subdirectory(features)
add_subdirectory(hardware_interface)
add_subdirectory(helper)
add_subdirectory(lib)
add_subdirectory(runtimes)

target_link_libraries(${SourceExecutable} PUBLIC ${SourceLib})
