Instructions for building:

Common steps across all platforms:
1. Download and install the ARM GNU toolchain compiler and debugger from https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
2. Download and install Ninja for your platform using instructions from here: https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages
3. Download and install CMake: https://cmake.org/download/
4. Once they are installed, restart your computer so environment variables take effect.
5. For Windows, install the Visual Studio Compiler
6. For Mac OS, install XCode
7. For Linux, the distribution you are using should come with a compiler - No extra steps necessary.

For the STM32 MCU:
 
1. In a terminal or IDE, CD to this directory and list the available presets:
``cmake --list-presets``
2. 2.Choose the ``arm-cortex-m7`` preset: ``cmake --preset arm-cortex-m7``
3. Once CMake successfully generates the build files, build the .elf binary using: ``cmake --build --preset build-arm``
4. Binary will be available in ``build/arm/src``.

For Windows Binary:
1. In a terminal or IDE, CD to this directory and list the available presets:
   ``cmake --list-presets``
2. Choose the ``native-x86_64-Visual-Studio`` preset: ``cmake --preset native-x86_64-Visual-Studio``
3. Once CMake successfully generates the build files, build the .elf binary using: ``cmake --build --preset build-x86_64-VS``
4. Binary will be available in ``build/arm/src``.

Mac OS & Linux:
1. TBD - have not tried on Mac or Linux yet. 