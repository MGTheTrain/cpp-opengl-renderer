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
- [ ] USD and/or glTF scene loading 
- [ ] PBR shader pipeline. 
- [ ] Support mainly for metal roughness workflow
- [ ] Integrated simple menu (e.g. with buttons, sliders and other widgets)
- [ ] Skeletal animation support (**OPTIONAL**)
- [ ] Port Desktop application to the web trough emscripten SDK (**OPTIONAL**)
- [ ] Consider compiled library for mobile support trough Java native interface (JNI) with kotlin and Swift native interface (SNI) with Swift (**OPTIONAL**)

## Getting started

### Preconditions

- Download and install [Visual Studio with components for building C and C++ apps](https://visualstudio.microsoft.com/vs/features/cplusplus/) on your target platform 
- Download and install [CMake](https://cmake.org/download/) 

### Debugging

Open the `cpp-opengl-renderer.sln`. Modify `include and library paths` in the cpp-opengl-renderer.vcxproj. Confirm the presence of only one entry point `int main()`. Set breakpoints as needed. Initiate the debugging process.

### CMake

TBD
