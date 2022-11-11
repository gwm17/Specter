# Example Project: SpecProject
SpecProject is an example project for use with Specter. It contains examples for making an analysis pipeline, extending the UI, and creating the executable entry point.
It also has an example CMakeLists.txt which demonstrates how to link a custom project to Specter.

## Building SpecProject
In this directory (the SpecProject directory) run the following commands:

- `mkdir build`
- `cd build && cmake .. && make -j 4`

This will build the SpecProject executable and install it to the Specter/bin directory. The executable should be run from the Specter/bin directory.