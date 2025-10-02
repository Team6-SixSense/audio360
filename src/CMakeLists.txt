# Source code CMakeLists.txt
enable_language(ASM)

set(SourceExecutable "Audio360.elf")
set(SourceLib "Audio360Lib")

# Collect source files.
file(GLOB_RECURSE SRC_FILES
    helper/*.cpp
    signal_processing/*.cpp
)
set(LIB_SRC_FILES ${SRC_FILES})
list(FILTER LIB_SRC_FILES EXCLUDE REGEX "main.cpp")

# Create a library from source code.
add_library(${SourceLib} STATIC ${LIB_SRC_FILES} )
target_sources(${SourceLib} PUBLIC ${LIB_SRC_FILES})

# Define executable.
add_executable(${SourceExecutable} main.cpp ${EXTRA_SOURCES})

set(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

# Attach include dirs to the target.
target_include_directories(${SourceExecutable} PUBLIC ${INCLUDE_DIRS} )
target_include_directories(${SourceLib} PUBLIC ${INCLUDE_DIRS} )

# Add definitions/includes if cross-compiling (toolchain sets them)
if(DEFINED EXTRA_DEFS)
    target_compile_definitions(${SourceExecutable} PRIVATE ${EXTRA_DEFS})
    target_compile_definitions(${SourceLib} PRIVATE ${EXTRA_DEFS})
endif()

if(DEFINED EXTRA_INCLUDES)
    target_include_directories(${SourceExecutable} PRIVATE ${EXTRA_INCLUDES})
    target_include_directories(${SourceLib} PRIVATE ${EXTRA_INCLUDES})
endif()

target_link_libraries(${SourceExecutable} PUBLIC
    ${SourceLib}
)
