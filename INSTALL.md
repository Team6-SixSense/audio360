# Instructions for building

## Required Software

### All platforms

1. Download and install the ARM GNU toolchain compiler and debugger from <https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads>.
2. Download and install Ninja for your platform using instructions from <https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages>.
3. Download and install CMake from <https://cmake.org/download/>.
4. Restart your computer so environment variables take effect.
5. Update all git submodules

   ```bash
   git submodule update --init --recursive
   ```

### Windows

1. Install the Visual Studio Compiler (MSVC)

### MacOS

1. Install XCode.

### Linux

1. The distribution you are using should come with a compiler; No extra steps necessary.

## Deploying onto STM32

1. Install clion. This will be used for debugging and deploying.
2. Download the latest of openOCD from <https://gnutoolchains.com/arm-eabi/openocd/>
3. Set the executable path of openOCD on clion.
4. link the .cfg file. `src/configs/st_nucleo_f7.cfg`
5. Set up GDB with interface and target config.
6. Set `arm-cortex-m7` as the cmake profile when building.

### Windows Note

1. Install the ST-LINK driver to communicate with the board via usb. <https://www.st.com/content/st_com/en/products/development-tools/software-development-tools/stm32-software-development-tools/stm32-utilities/stsw-link009.html#get-software>
2. Follow the instruction in the readme.txt from the downloaded folder.
