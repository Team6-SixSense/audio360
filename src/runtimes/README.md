# Runtimes Code

This folder contains alternative runtime source files.
Each file behaves similarly to a main.cpp, but is included as a selectable runtime module instead of being the primary entry point.

These modules are conditionally compiled using `#ifdef` directives in the parent main.cpp.
This setup allows the team to easily switch between different runtime implementations, such as debugging runtimes versus production runtimes,without modifying large portions of code.
Switching only requires defining the appropriate `#ifdef` macro at compile time.
