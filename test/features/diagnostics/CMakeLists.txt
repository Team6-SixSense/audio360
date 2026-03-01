# test/features/diagnostics CMakeLists.txt

# Define test executable files.
target_sources(${TestExecutable} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/system_fault_manager_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/system_fault_state_machine_test.cpp
)
