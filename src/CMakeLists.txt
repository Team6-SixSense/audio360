# Source code CMakeLists.txt

set(SourceExecutable "Audio360")
set(SourceLib "Audio360Lib")

include(FetchContent)
FetchContent_Declare(
    matplotplusplus
    GIT_REPOSITORY https://github.com/alandefreitas/matplotplusplus
    GIT_TAG origin/master
) 
        
FetchContent_GetProperties(matplotplusplus)
if(NOT matplotplusplus_POPULATED)
    FetchContent_Populate(matplotplusplus)
    add_subdirectory(${matplotplusplus_SOURCE_DIR} ${matplotplusplus_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

add_subdirectory(lib/fftw-3.3.10/)

# set(FFTW3_DIR "${CMAKE_SOURCE_DIR}/build/src/lib/fftw-3.3.10")


# Collect source files.
file(GLOB_RECURSE SRC_FILES *.cpp)
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
add_library(${SourceLib} ${LIB_SRC_FILES})
target_link_libraries(${SourceExecutable} PRIVATE ${SourceLib})
target_link_libraries(${SourceExecutable} PUBLIC 
matplot
fftw3)
target_link_libraries(${SourceLib} PUBLIC matplot)
target_link_libraries(${SourceLib} PUBLIC fftw3)

# Attach include dirs to the target.
target_include_directories(${SourceExecutable} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${SourceLib} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})