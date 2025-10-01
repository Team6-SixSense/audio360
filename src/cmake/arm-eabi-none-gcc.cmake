# =========================
# Toolchain: ARM bare-metal (STM32F7 / Cortex-M7)
# =========================

# Tell CMake we're cross-compiling for a non-host (no OS) target.
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)


# Critical: don't try to link a host exe during compiler checks.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(STARTUP ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/CMSIS/Device/ST/STM32F7xx/Source/Templates/gcc/startup_stm32f767xx.s)
set(SYSTEM  ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/CMSIS/Device/ST/STM32F7xx/Source/Templates/system_stm32f7xx.c)
# implements SystemInit(), clocks, caches

# --- Where to find the GNU ARM toolchain ---
# If the toolchain's bin/ is already on PATH, you can leave ARM_GCC_BIN empty.
# Otherwise, set it here or pass -DARM_GCC_BIN="C:/Program Files (x86)/Arm GNU Toolchain arm-none-eabi/14.3 rel1/bin"
set(ARM_GCC_BIN "" CACHE PATH "Path to arm-none-eabi-* binaries (bin directory)")

# Compilers and binutils
set(CMAKE_C_COMPILER   "${ARM_GCC_BIN}/arm-none-eabi-gcc"   CACHE FILEPATH "" FORCE)
set(CMAKE_CXX_COMPILER "${ARM_GCC_BIN}/arm-none-eabi-g++"   CACHE FILEPATH "" FORCE)
set(CMAKE_ASM_COMPILER "${ARM_GCC_BIN}/arm-none-eabi-gcc"   CACHE FILEPATH "" FORCE)

find_program(CMAKE_AR           arm-none-eabi-ar    HINTS "${ARM_GCC_BIN}")
find_program(CMAKE_RANLIB       arm-none-eabi-ranlib HINTS "${ARM_GCC_BIN}")
find_program(CMAKE_STRIP        arm-none-eabi-strip HINTS "${ARM_GCC_BIN}")

# Useful post-build tools (optional; referenced in your main CMakeLists)
find_program(ARM_SIZE    arm-none-eabi-size    HINTS "${ARM_GCC_BIN}")
find_program(ARM_OBJCOPY arm-none-eabi-objcopy HINTS "${ARM_GCC_BIN}")
find_program(ARM_OBJDUMP arm-none-eabi-objdump HINTS "${ARM_GCC_BIN}")

# --- CPU / FPU flags (exposed for your targets to use) ---
# STM32F7 = Cortex-M7 with single-precision FPU.
set(ARM_CPU        "cortex-m7"        CACHE STRING "ARM CPU")
set(ARM_FPU        "fpv5-sp-d16"      CACHE STRING "ARM FPU")
set(ARM_FLOAT_ABI  "hard"             CACHE STRING "ARM float ABI (hard|softfp|soft)")

set(ARM_MCU_FLAGS "-mcpu=${ARM_CPU} -mthumb -mfpu=${ARM_FPU} -mfloat-abi=${ARM_FLOAT_ABI}"
    CACHE STRING "Common per-target MCU flags")

set(CMAKE_C_FLAGS_INIT   "${ARM_MCU_FLAGS} -ffunction-sections -fdata-sections -Wall -Wextra")
set(CMAKE_CXX_FLAGS_INIT "${ARM_MCU_FLAGS} -ffunction-sections -fdata-sections -Wall -Wextra -fno-exceptions -fno-rtti")
set(CMAKE_ASM_FLAGS_INIT "${ARM_MCU_FLAGS}")

# Linker script (adjust path if needed)
set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/../cmake/stm32-f7.ld")

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS_INIT
        "${ARM_MCU_FLAGS} --specs=nano.specs --specs=nosys.specs \
    -T${LINKER_SCRIPT} -Wl,-Map=Audio360.map -Wl,--gc-sections"
)


# Add ARM-specific startup/system sources
set(STARTUP_FILE
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/CMSIS/Device/ST/STM32F7xx/Source/Templates/gcc/startup_stm32f767xx.s
        CACHE FILEPATH "Startup assembly file"
)

set(SYSTEM_FILE
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/CMSIS/Device/ST/STM32F7xx/Source/Templates/system_stm32f7xx.c
        CACHE FILEPATH "SystemInit source file"
)

# Export to parent CMakeLists.txt
set(EXTRA_SOURCES ${STARTUP_FILE} ${SYSTEM_FILE} CACHE INTERNAL "")
set(EXTRA_DEFS STM32F7 STM32F767xx ARM_MATH_CM7 STM_BUILD CACHE INTERNAL "")
set(EXTRA_INCLUDES
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/CMSIS/Device/ST/STM32F7xx/Include
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/CMSIS/Include
        CACHE INTERNAL ""
)

# Keep find_path/find_library from searching host locations by default.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# (Optional) Default C/C++ standards for convenience
set(CMAKE_C_STANDARD   11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)
