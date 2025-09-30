# Source code CMakeLists.txt

set(SourceExecutable "Audio360")
set(SourceLib "Audio360Lib")

# Collect source files.
file(GLOB SRC_FILES *.cpp)
set(LIB_SRC_FILES ${SRC_FILES})
list(FILTER LIB_SRC_FILES EXCLUDE REGEX "main.cpp")

# Define executable.
if(NOT CI_RUN)
    add_executable(${SourceExecutable} main.cpp)
else()
    # Build as static library for CI to skip linking steps.
    add_library(${SourceExecutable} STATIC main.cpp)
endif()

# Create a library from source code.
# TODO: uncomment once we have source code.
# add_library(${SourceLib} ${LIB_SRC_FILES})
# target_link_libraries(${SourceExecutable} PRIVATE ${SourceLib})

# Attach include dirs to the target.
target_include_directories(${SourceExecutable} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# TODO: uncomment once we have source code.
# target_include_directories(${SourceLib} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
