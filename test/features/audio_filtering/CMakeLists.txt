# test/features/audio_filtering CMakeLists.txt

# Define test executable files.
target_sources(${TestExecutable} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/spectral_leakage_test.cpp
)
