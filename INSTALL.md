# Instructions for building

## Required Software

### All platforms

1. Download and install the ARM GNU toolchain compiler and debugger from <https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads>.
2. Download and install Ninja for your platform using instructions from <https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages>.
3. Download and install CMake from <https://cmake.org/download/>.
4. Restart your computer so environment variables take effect.

### Windows

1. Install the Visual Studio Compiler (MSVC)

### MacOS

1. Install XCode.

### Linux

1. The distribution you are using should come with a compiler; No extra steps necessary.

## Building

### Building for STM32 MCU (target hardware)

1. Ensure the path to ARM gcc compiler is included in `PATH` environment variable. Alternatively set `ARM_GCC_BIN` cmake variable manually.

   ```text
   example path: C:/Program Files (x86)/Arm GNU Toolchain arm-none-eabi/14.3 rel1/bin
   ```

2. In a terminal or IDE, CD to this directory and list the available presets.

   ```bash
   cmake --list-presets
   ```

3. Choose the `arm-cortex-m7` preset.

   ```bash
   cmake --preset arm-cortex-m7
   ```

4. Once CMake successfully generates the build files, build the .elf binary using the following.

   ```bash
   cmake --build --preset build-arm
   ```

5. The binary will be available in `build/arm/src/<release_type>`.

### Building for Windows (Testing purposes)

1. In a terminal or IDE, CD to this directory and list the available presets:

   ```bash
   cmake --list-presets
   ```

2. Choose the `native-x86_64-Visual-Studio` preset.

   ```bash
   cmake --preset native-x86_64-Visual-Studio
   ```

3. Once CMake successfully generates the build files, build the .elf binary using the following.

   ```bash
   cmake --build --preset build-x86_64-VS
   ```

4. Binary will be available in `build/x86_64/src/<release_type>`.
