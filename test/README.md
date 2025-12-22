# Automated tests

This directory contains automated tests for the software components implemented in the `src` folder.
The directory structure mirrors the src folder exactly to maintain a clear correspondence between source files and their associated tests.

Each test file is named using the convention `<component>_test.cpp` and contains unit tests for the corresponding component.

The tests are implemented using Google Test (gtest). To build and run the test suite, configure the project with the BUILD_TESTS flag enabled during the build process.
