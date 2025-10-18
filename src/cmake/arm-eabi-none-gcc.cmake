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

set(STARTUP ${CMAKE_CURRENT_LIST_DIR}/../configs/startup_stm32f767zitx.s)
set(SYSTEM ${CMAKE_CURRENT_LIST_DIR}/../configs/system_stm32f7xx.c)
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

set(CMAKE_C_FLAGS_INIT   "${ARM_MCU_FLAGS} -ffunction-sections -fdata-sections -ffunction-sections -fdata-sections -fno-common -fmessage-length=0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_INIT "${ARM_MCU_FLAGS} -ffunction-sections -fdata-sections -fno-common -fmessage-length=0 -Wall -Wextra -fno-exceptions -fno-rtti")
set(CMAKE_ASM_FLAGS_INIT "${ARM_MCU_FLAGS} -x assembler-with-cpp")

# Linker script (adjust path if needed)
set(LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/../configs/STM32F767ZITX_FLASH.ld)

# Linker flags
add_link_options(-Wl,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
add_link_options(-mcpu=cortex-m7 -mthumb -mthumb-interwork)
add_link_options(--specs=nosys.specs)
add_link_options(-T ${LINKER_SCRIPT})

add_compile_definitions(__FPU_PRESENT=1 __FPU_USED=1)

set(HARDWARE_SRC_FILE
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/BSP/STM32F7xx_Nucleo_144/stm32f7xx_nucleo_144.c

        # Peripherals functions.
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_adc.c
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_adc_ex.c
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_cortex.c
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma.c
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_gpio.c
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr_ex.c
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr.c
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc.c
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc_ex.c
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_spi.c
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_uart.c
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal.c

        # SAI peripheral
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_sai.c

        # FFT functions.
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/CMSIS/DSP/Source/TransformFunctions/arm_rfft_fast_f32.c
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/CMSIS/DSP/Source/TransformFunctions/arm_rfft_fast_init_f32.c
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/CMSIS/DSP/Source/TransformFunctions/arm_cfft_f32.c
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/CMSIS/DSP/Source/TransformFunctions/arm_cfft_radix8_f32.c
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/CMSIS/DSP/Source/TransformFunctions/arm_rfft_init_q15.c
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/CMSIS/DSP/Source/TransformFunctions/arm_rfft_init_q31.c
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/CMSIS/DSP/Source/CommonTables/arm_common_tables.c
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/CMSIS/DSP/Source/CommonTables/arm_const_structs.c
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/src/TransformFunctions/bitreversal.c

        CACHE FILEPATH "UART printf source files"
)

# Export to parent CMakeLists.txt
set(EXTRA_SOURCES ${STARTUP} ${SYSTEM} ${HARDWARE_SRC_FILE} CACHE INTERNAL "")
set(EXTRA_DEFS STM32F7 STM32F767xx ARM_MATH_CM7 STM_BUILD USE_HAL_DRIVER HAL_SAI_MODULE_ENABLED CACHE INTERNAL "")
set(EXTRA_INCLUDES
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/BSP/STM32F7xx_Nucleo_144

        # Peripherals functions.
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/STM32F7xx_HAL_Driver/Inc
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/CMSIS/Device/ST/STM32F7xx/Include

        # FFT functions.
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/CMSIS/Include
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/CMSIS/Core/Include
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/CMSIS/DSP/Include
        ${CMAKE_CURRENT_LIST_DIR}/../STM32CubeF7/Drivers/CMSIS/DSP/DSP_Lib_TestSuite/RefLibs/inc

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
