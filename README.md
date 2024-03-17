# cpp-opengl-renderer

## Table of Contents

+ [Summary](#summary)
+ [Features](#features)
+ [Getting started](#getting-started)

## Summary

A cross-platform C++ OpenGL renderer

## Features

- [x] Compilation on MacOS, Linux Ubuntu 20.04/22.04, Linux debian 11 and Windows 10/11 OS
- [x] Support for glTF scene loading
- [x] PBR shader pipeline considering primarily the metal roughness workflow
- [x] High code coverage with GoogleTest for methods in module structs and classes 
- [x] Create `CMakeLists.txt` files for cross-platform build environment generation
- [x] CI workflows ensuring cross-platform compilation primarily for Desktop operating systems 
- [x] CI workflows ensuring continuous testing of the modules 
- [x] Cross-platform CPack packaging for Linux, MacOS and Windows OSes
- [x] Integrated simple menu (e.g. with sliders and other widgets)
- [x] Native file dialog support for loading new glTF scenes
- [ ] ~~PBR fragment shader with switchable `fragmentColor` for inspecting intermediate outcomes (**OPTIONAL**)~~
- [ ] ~~Support for USD scene loading (**OPTIONAL**)~~
- [ ] ~~Skeletal animation support (**OPTIONAL**)~~
- [x] Port Desktop application to the web trough emscripten SDK (**OPTIONAL**)
- [ ] Consider compiled library for mobile support trough Java native interface (JNI) with kotlin and Swift native interface (SNI) with Swift (**OPTIONAL**)

## Getting started

### Preconditions

- Download and install [Visual Studio with components for building C and C++ apps](https://visualstudio.microsoft.com/vs/features/cplusplus/) on your target platform 
- Download and install [CMake from the official website](https://cmake.org/download/)  or trough a package manager
- Install [vcpkg from the official website](https://vcpkg.io/en/getting-started.html) or trough a package manager
- To obtain multiple glTF samples, ensure that you update the Git submodules: `git submodule update --init` 
- For web builds the `emscripten SDK` needs to be installed. [Click here](https://github.com/emscripten-core/emsdk)

### Integrate vcpkg with Visual Studio

Open a command prompt and navigate to the vcpkg directory. Run the following command to integrate vcpkg with Visual Studio: `<vcpkg installation path>/vcpkg integrate install`
Run the following command from the vcpkg directory to install the dependencies specified in the vcpkg.json file: `<vcpkg installation path>/vcpkg install`


#### Mac OS

Ensure on MacOS you have installed [brew](https://brew.sh) with `/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"`

Add brew to the `PATH` variable and substitute `<your user>` for:
```sh
`(echo; echo 'eval "$(/opt/homebrew/bin/brew shellenv)"') >> /Users/<your user>/.zprofile
    eval "$(/opt/homebrew/bin/brew shellenv)"
```

Thereafter install the required brew packages/formulas with `brew install pkg-config cmake` 

### Debugging

Open the `cpp-opengl-renderer.sln`. Modify `include  pathes, library pathes and library names` in the `cpp-opengl-renderer.vcxproj`. Confirm the presence of only one entry point `int main()`. Set breakpoints as needed. Initiate the debugging process. 
If you encounter DLL-related errors, make sure to copy the necessary dynamic libraries to the directory where the built executable is located and then rebuild.

### Google Test build configurations

Make sure to configure the project settings appropriately for the `Debug|x64` build configuration by setting the correct library path and copying the required DLLs:

- **Library path:** `<vcpkg installation path>/vcpkg/packages/gtest_x64-windows/debug/lib`
- **Binary path:** `<vcpkg installation path>/vcpkg/packages/gtest_x64-windows/debug/bin`

Similair for `Release|x64` build configurations:

- **Library path:** `<vcpkg installation path>/vcpkg/packages/gtest_x64-windows/lib`
- **Binary path:** `<vcpkg installation path>/vcpkg/packages/gtest_x64-windows/bin`

### CMake

#### Unix

In order to compile the source code and link the binaries run in a Unix terminal:

```sh
cd devops/scripts/bash/
./compile_source_code.sh -CMakeToolchainFile <path to the vcpkg.cmake file>
```

For web builds ensure the `emscripten SDK` is installed and then run:

```sh
cd devops/scripts/bash/
source <Path to emsdk folder>/emsdk_env.sh
./compile_source_code.sh -WebBuild 
# Copy build artifacts (.js, .wasm, .data) over to assets\web\html-js-wasm
# Start the dev container within the `.devcontainer/nginx` directory. Once inside the container, copy the contents of the assets/web/html-js-wasm directory to /usr/share/nginx/html
# Visit in a browser of choice `localhost:8090/rotating_texture_cube.html` or `localhost:8090/opengl_viewer.html`
```

In order to additionaly start the test executables in a Unix terminal: 

```sh
cd devops/scripts/bash/
./compile_source_code.sh -CMakeToolchainFile <path to the vcpkg.cmake file> -RunTests
```

In order to create compressed files or packages:

```sh
cd build/
cpack -G <DEB, ZIP, TGZ, TXZ> # DEB only for Linux systems. Check further generators for the specific Unix OS and CPU arch via `cpack -G --help` 
cd ../packages
# install package or unzip compressed file
```

#### Windows

In order to compile the source code and link the binaries run in a PowerShell terminal:

```sh
cd devops\scripts\ps1
.\compile_source_code.ps1 -CMakeToolchainFile <path to the vcpkg.cmake file>
```

For web builds ensure the `emscripten SDK` is installed, `ninja` or `mingw32-make` are available and then run:

```sh
cd devops\scripts\ps1
source <Path to emsdk folder>\emsdk_env.bat
.\compile_source_code.ps1 -WebBuild
# Copy build artifacts (.js, .wasm, .data) over to assets/web/html-js-wasm
# Start the dev container within the `.devcontainer/nginx` directory. Once inside the container, copy the contents of the assets/web/html-js-wasm directory to /usr/share/nginx/html
# Visit in a browser of choice `localhost:8090/rotating_texture_cube.html` or `localhost:8090/opengl_viewer.html`
```

In order to additionaly start the test executables in a PowerShell terminal: 

```sh
cd devops\scripts\ps1
.\compile_source_code.ps1 -CMakeToolchainFile <path to the vcpkg.cmake file> -RunTests
```

In order to create compressed files or installers:

```sh
cd build/
cpack -G <ZIP, NSIS, NSIS64> # Check further generators for later Windows OSes and CPU arch via `cpack -G --help` 
cd ../packages
# run installer or unzip compressed file
```

## Apps

Images of the rendered scenes can be viewed here:

- [opengl-viewer README.md](./apps/opengl-viewer/README.md)
- [rotating-textured-cube README.md](./apps/rotating-textured-cube/README.md)
