# Specter

Specter is a cross-platform Dear ImGui/ImPlot based data visualization tool designed for online analysis in nuclear physics experiments. It is a C++ library/framework with capability of displaying 1D and 2D
spectra, applying cuts and gates graphically to spectra, and has a customizable analysis system. Specter is focused on providing a clean, pretty UI with good performance required by demainding experimental setups.

The only external dependencies are OpenGL development headers, as OpenGL is the rendering API, and the
C++17 standard library. All other dependencies are included as git submodules in the vendor directory. The current state of the library is such that the only readily developed data source type is for CAEN's
CoMPASS DAQ for use with their digitizer modules. There are tentative plans to extend the sources to other DAQ frameworks.

This project would not be possible with out the amazing online tutorials of [@TheCherno](https://github.com/TheCherno) and in particular his Hazel tutorials, which much of the application model and basically
all of the renderer is based off of ([Hazel](https://github.com/TheCherno/Hazel)). I highly recommend checking out his work to anyone who is interested in projects like this, or just learning more about C++.

Additonally, the excellent [DearImGui](https://github.com/ocornut/imgui) and [ImPlot](https://github.com/epezent/implot) libraries are obviously critical to this work, and also deserve a look for anyone who wants
to design UI's and plotting tools in C++.

## Building Specter

Specter should be cloned from github using `git clone --recursive https://github.com/gwm17/Specter.git` The recursive flag tells git to clone not only the repository but also all of the submodule dependencies.

Specter uses [premake](https://premake.github.io/), a free, cross-platform build tool to generate build files for various platforms.

Note: On Linux distributions, typically Mesa OpenGL and X-window related header files are not installed by default. These can be installed using whatever package manager your distribution uses. For example,
on Debian family distributions the necessary files can be installed using `sudo apt install libgl1 libgl1-mesa-dev libglu1-mesa libglu1-mesa-dev xorg-dev mesa-utils` which should fill out all of
the dependencies. If this doesn't seem to work, check your distribution related documentation for OpenGL dependencies.

## Running Specter

Once Specter is built it can be run from VS or Xcode IDEs or on Linux move to the appropriate directory in bin and execute the program. Note that the program is SpecProject. Specter is just a library/framework.
SpecProject is an example showing what a typical implementation might look like, based on the Super-Enge Split-Pole Spectrograph at Florida State University. This can be used as a launching point for a project, or
you can link to Specter your self. Note that if you want to link to Specter yourself, its important that you copy the fonts directory to your project, as this contains the icons and font used by Specter. The
fonts should be placed wherever your program will be run from (the runtime directory).

Note: As Specter is still very early in development, the default build configuration is Debug. If you are actually going to use it, you will want to make sure that you switch to building in Release configuration
otherwise performance may be poor.