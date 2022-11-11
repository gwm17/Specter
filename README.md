# Specter

Specter is a cross-platform Dear ImGui/ImPlot based data visualization tool designed for online analysis in nuclear physics experiments. It is a C++ library/framework with capability of displaying 1D and 2D
spectra, applying cuts and gates graphically to spectra, and has a customizable analysis system. Specter is focused on providing a clean, pretty UI with good performance required by demainding experimental setups.

The only external dependencies are OpenGL development headers, as OpenGL is the rendering API, and the
C++20 standard library. All other dependencies are included as git submodules in the vendor directory.

This project would not be possible with out the amazing online tutorials of [@TheCherno](https://github.com/TheCherno) and in particular his Hazel tutorials, which much of the application model and basically
all of the renderer is based off of ([Hazel](https://github.com/TheCherno/Hazel)). I highly recommend checking out his work to anyone who is interested in projects like this, or just learning more about C++.

Additonally, the excellent [DearImGui](https://github.com/ocornut/imgui) and [ImPlot](https://github.com/epezent/implot) libraries are obviously critical to this work, and also deserve a look for anyone who wants
to design UI's and plotting tools in C++.

## Building Specter

Specter should be cloned from github using `git clone --recursive https://github.com/gwm17/Specter.git` The recursive flag tells git to clone not only the repository but also all of the submodule dependencies.

Specter uses CMake as its build system. If you are unfamiliar with CMake, building Specter is very straightforward. From within the Specter repository run the following commands:

- `mkdir build`
- `cd build && cmake .. && make -j 4 && make install`

This builds and installs the Specter library. By default, Specter is installed into the local repository in the lib directory. If you want to install Specter along global resource paths, you will need to modify
the Specter CMakeLists.txt.

See the SpecProject README for information on how to build the example project (and link Specter to a user made project).

Note: On Linux distributions, typically Mesa OpenGL and X-window related header files are not installed by default. These can be installed using whatever package manager your distribution uses.
For example, on Debian family distributions the necessary files can be installed using `sudo apt install libgl1 libgl1-mesa-dev libglu1-mesa libglu1-mesa-dev xorg-dev mesa-utils` which should fill out all of the
dependencies. If this doesn't seem to work, check your distribution related documentation for OpenGL an X11 dependencies.

## Using Specter
Specter is simply a library framework for making a GUI data analysis program. In the Specter repository there is an example of a project using Specter (SpecProject) designed around the SESPS analysis pipeline at
Florida State University. This can be used as a template from which more complex projects can be built. Typically, one would install Specter, and then create a separate project for the actual program. To link Specter using
CMake, one can use the `find_package()` command. If the standard install is run, you will also need to pass the `PATHS <your_path_to_Specter>` argument to the `find_package()` command. See SpecProject README and CMakeLists for more details.

Note that your project will need to copy the Assets folder (located in SpecProject) to the runtime location of your project executable.

## Suppported data sources
Specter supports CAEN data sources (binary data files fully supported, online is a work in progress) and Daqromancy data sources (untested as of yet) and nscldaq data transmitted by the Charon server.

### Status as of Aug 2022
Unfortunately, at the time of writing, CoMPASS does not fully support the use of external data analysis. CoMPASS versions 1.3-2.0 have a data socket, which can not be accessed outside of the localhost,
meaning that use with Specter generally requires an additional server broadcasting the data to the machine running Specter, which is non-trivial. CoMPASS began supporting a public data socket as of version 2.1,
however, it is now by default a time-sorted data socket. The time-sorting operation is so inefficient that any significant data rate on the aquisition basically results in a soft crash of CoMPASS.
Currently there is no option to turn off the time sorting on the data socket. CAEN has been alerted to this issue, so hopefully this will be rectified shortly.