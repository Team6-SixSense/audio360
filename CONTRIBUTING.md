# Contributing

## Pre-requisites

1. Ability to compile C/C++ code on your machine. Example commands provided below.

    ```bash
    cmake -B build -S .
    cmake -G "MinGW Makefiles" -B build -S . # Run this if working on Windows

    cmake --build build --config Release
    ```

## Coding Guidelines

This project uses the following coding guidelines for the programming languages being used.

| Programming Language | Coding Standard |
| -------------------- | --------------- |
| C | [GNU C](https://www.gnu.org/prep/standards/html_node/Writing-C.html) |
| C++ | [Google C++](https://google.github.io/styleguide/cppguide.html) |
| Python | [PEP8](https://peps.python.org/pep-0008/) |

To adhere to the coding standards, use the suggested autoformatters and linters used in this project.

### clang-format

[clang-format](https://clang.llvm.org/docs/ClangFormat.html) is a tool to auto-format C/C++ code.

1. Run `pip install -r requirements.txt`. clang-format is provided by Python pip.
2. Install `Clang-Format by Xaver Hellauer` Visual Studio Code extension. This tool will auto-format on save when working in Visual Studio Code.

### C/C++ Linter

Use Microsoft’s [C/C++ extension](https://code.visualstudio.com/docs/languages/cpp) from Visual Studio Code.

### Python Linter

[pylint](https://pypi.org/project/pylint/) is the linter of choice for Python code in this project. If working in Visual Studio Code, use their [Pylint extension](https://marketplace.visualstudio.com/items?itemName=ms-python.pylint).

## Creating a PR

Create a PR on github when contributing and set the reviewers to be one of the project owners mentioned in the main [README](README.md).
Follow the PR template provided and ensure all tests are passing.
Link the issue in the PR; the issue should be detailed and describe what is being changed and why.
