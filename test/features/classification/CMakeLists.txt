# test/features/classification CMakeLists.txt

# Define test executable files.
target_sources(${TestExecutable} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/mel_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dct_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pca_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lda_test.cpp
)
