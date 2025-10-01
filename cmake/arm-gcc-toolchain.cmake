# Minimal toolchain file for CI builds with ARM GCC (no linker script needed)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Use Arm GCC cross-compiler
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

# Prevent CMake from trying to run test binaries
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Just basic compile flags (no hardware specifics)
set(CMAKE_C_FLAGS_INIT "-O2 -mcpu=cortex-m4 -mthumb")
set(CMAKE_CXX_FLAGS_INIT "-O2 -mcpu=cortex-m4 -mthumb -fno-exceptions -fno-rtti")
