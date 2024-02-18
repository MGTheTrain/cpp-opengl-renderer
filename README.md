# cpp-opengl-renderer

## Table of Contents

+ [Summary](#summary)
+ [Features](#features)
+ [References](#references)
+ [Getting started](#getting-started)

## Summary

A cross-platform C++ OpenGL renderer

## References

TBD

## Features

- [ ] Compilation on MacOS, Linux Ubuntu 20.04/22.04, Linux debian 11 and Windows 10/11 OS
- [x] Support for glTF scene loading
- [x] PBR shader pipeline considering primarily the metal roughness workflow
- [ ] Integrated simple menu (e.g. with buttons, sliders and other widgets)
- [ ] High code coverage with GoogleTest for methods in module structs and classes 
- [ ] Create `CMakeLists.txt` files for cross-platform build environment generation
- [ ] CI/CT workflows ensuring cross-platform compilation primarily for Desktop operating systems and continuous testing of the modules
- [ ] Native file dialog support
- [ ] PBR fragment shader with switchable `fragmentColor` for inspecting intermediate outcomes (**OPTIONAL**)
- [ ] Support for USD scene loading (**OPTIONAL**)
- [ ] Skeletal animation support (**OPTIONAL**)
- [ ] Port Desktop application to the web trough emscripten SDK (**OPTIONAL**)
- [ ] Consider compiled library for mobile support trough Java native interface (JNI) with kotlin and Swift native interface (SNI) with Swift (**OPTIONAL**)

## Getting started

### Preconditions

- Download and install [Visual Studio with components for building C and C++ apps](https://visualstudio.microsoft.com/vs/features/cplusplus/) on your target platform 
- Download and install [CMake from the official website](https://cmake.org/download/)  or trough a package manager
- Install [vcpkg from the official website](https://vcpkg.io/en/getting-started.html) or trough a package manager
- To obtain multiple glTF samples, ensure that you update the Git submodules: `git submodule update --init` 

### Integrate vcpkg with Visual Studio

Open a command prompt and navigate to the vcpkg directory. Run the following command to integrate vcpkg with Visual Studio: `<vcpkg installation path>/vcpkg integrate install`
Run the following command from the vcpkg directory to install the dependencies specified in the vcpkg.json file: `<vcpkg installation path>/vcpkg install`

### Debugging

Open the `cpp-opengl-renderer.sln`. Modify `include  pathes, library pathes and library names` in the `cpp-opengl-renderer.vcxproj`. Confirm the presence of only one entry point `int main()`. Set breakpoints as needed. Initiate the debugging process. 
If you encounter DLL-related errors, make sure to copy the necessary dynamic libraries to the directory where the built executable is located and then rebuild.

### Google Test build configurations

Make sure to configure the project settings appropriately for the `Debug|x64` build configuration by setting the correct library path and copying the required DLLs:

**Library path:** `<vcpkg installation path>/vcpkg/packages/gtest_x64-windows/debug/lib`
**Binary path:** `<vcpkg installation path>/vcpkg/packages/gtest_x64-windows/debug/bin`

Similair for `Release|x64` build configurations:

**Library path:** `<vcpkg installation path>/vcpkg/packages/gtest_x64-windows/lib`
**Binary path:** `<vcpkg installation path>/vcpkg/packages/gtest_x64-windows/bin`

### CMake

TBD

## Apps

Images of the rendered scenes can be viewed here:

- [opengl-viewer README.md](./apps/opengl-viewer/README.md)
- [rotating-textured-cube README.md](./apps/rotating-textured-cube/README.md)
