# test/features CMakeLists.txt

# Add subdirectories (each adds sources/includes).
add_subdirectory(audio_filtering)
add_subdirectory(diagnostics)
add_subdirectory(doa)
add_subdirectory(signal_processing)
add_subdirectory(classification)
add_subdirectory(audio360_engine)
add_subdirectory(integration)

# Define test executable files.
target_sources(${TestExecutable} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/packet_test.cpp
)