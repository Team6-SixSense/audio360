# =========================
# Toolchain: ARM bare-metal (STM32F7 / Cortex-M7)
# =========================

# Tell CMake we're cross-compiling for a non-host (no OS) target.
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Disable tests for ARM build.
set(BUILD_TESTS OFF CACHE BOOL "ON to build tests" FORCE)

# Critical: don't try to link a host exe during compiler checks.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(STARTUP ${CMAKE_CURRENT_LIST_DIR}/stm_startup/stm32f767_startup.s)
set(SYSTEM ${CMAKE_CURRENT_LIST_DIR}/stm_startup/system_stm32f767.c)
# implements SystemInit(), clocks, caches

# --- Where to find the GNU ARM toolchain ---
# If the toolchain's bin/ is already on PATH, you can leave ARM_GCC_BIN empty.
# Otherwise, set it here or pass -DARM_GCC_BIN="C:/Program Files (x86)/Arm GNU Toolchain arm-none-eabi/14.3 rel1/bin"
set(ARM_GCC_BIN "" CACHE FILEPATH "Path to arm-none-eabi-* binaries (bin directory)")

# file extension on Windows
if(WIN32)
    set(_EXT ".exe")
else()
    set(_EXT "")
endif()


if(ARM_GCC_BIN AND NOT "${ARM_GCC_BIN}" STREQUAL "")
    message(STATUS "Searching toolchain in: ${ARM_GCC_BIN}")
    find_program(ARM_GCC_CC
            NAMES arm-none-eabi-gcc arm-none-eabi-gcc.exe
            PATHS "${ARM_GCC_BIN}"
            NO_DEFAULT_PATH)
    find_program(ARM_GXX_CC
            NAMES arm-none-eabi-g++ arm-none-eabi-g++.exe
            PATHS "${ARM_GCC_BIN}"
            NO_DEFAULT_PATH)
else()
    message(STATUS "Searching toolchain in PATH")
    find_program(ARM_GCC_CC
            NAMES arm-none-eabi-gcc arm-none-eabi-gcc${_EXT})
    find_program(ARM_GXX_CC
            NAMES arm-none-eabi-g++ arm-none-eabi-g++${_EXT})
endif()


if(NOT ARM_GCC_CC)
    message(FATAL_ERROR "Could not find arm-none-eabi-gcc. Install GNU Arm Embedded toolchain or set -DARM_GCC_BIN=/path/to/bin")
endif()
if(NOT ARM_GXX_CC)
    message(FATAL_ERROR "Could not find arm-none-eabi-g++.")
endif()

# Force CMake to use these compilers (cache them so they persist)
set(CMAKE_C_COMPILER   "${ARM_GCC_CC}"  CACHE FILEPATH "C compiler"   FORCE)
set(CMAKE_CXX_COMPILER "${ARM_GXX_CC}"  CACHE FILEPATH "C++ compiler" FORCE)
# if you need assembler driver set it too (gcc works as assembler driver)
set(CMAKE_ASM_COMPILER "${ARM_GCC_CC}"  CACHE FILEPATH "ASM compiler"  FORCE)

message(STATUS "Using C compiler: ${CMAKE_C_COMPILER}")
message(STATUS "Using CXX compiler:  ${CMAKE_CXX_COMPILER}")




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



# Export to parent CMakeLists.txt
set(EXTRA_SOURCES ${STARTUP} ${SYSTEM} CACHE INTERNAL "")
set(EXTRA_DEFS STM32F7 STM32F767xx ARM_MATH_CM7 STM_BUILD CACHE INTERNAL "")
set(EXTRA_INCLUDES
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/CMSIS/Device/ST/STM32F7xx/Include
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/CMSIS/Include
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/CMSIS/Core/Include
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/CMSIS/DSP/Include
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
