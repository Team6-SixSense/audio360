# src/features/classification CMakeLists.txt

target_sources(${SourceLib} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/classification.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dct.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lda.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mel_filter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pca.cpp
)

# Add include directories.
target_include_directories(${SourceLib} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
