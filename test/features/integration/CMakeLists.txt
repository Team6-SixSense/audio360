# test/features/integration CMakeLists.txt

# Define test executable files.
target_sources(${TestExecutable} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/frequency_analysis_integration_test.cpp
)
