# cpp-opengl-renderer

## Summary

A cross-platform C++ OpenGL renderer supporting glTF scene loading with a PBR metal-roughness shader pipeline.

## Features

- [x] Compilation on macOS, Ubuntu 20.04/22.04, Debian 11, Windows 10/11
- [x] glTF scene loading via tinygltf
- [x] PBR shader pipeline (metal-roughness workflow)
- [x] Image-based lighting (IBL) with equirectangular HDR environment maps
- [x] ImGui settings panel (sliders, checkboxes, native file dialog for scene selection)
- [x] GoogleTest coverage for module structs and classes
- [x] CMake + vcpkg cross-platform build system
- [x] CI workflows for cross-platform compilation and continuous testing
- [x] CPack packaging for Linux (DEB, TGZ), macOS, and Windows (ZIP, NSIS)
- [x] Web port via Emscripten
- [ ] PBR fragment shader with switchable `fragmentColor` for inspecting intermediate outputs *(optional)*
- [ ] USD scene loading *(optional)*
- [ ] Skeletal animation *(optional)*
- [ ] Mobile support via JNI (Kotlin) and Swift native interface *(optional)*

## Prerequisites

- CMake ≥ 3.10
- vcpkg (bootstrapped via `third-party/vcpkg` submodule)
- For glTF sample scenes: `git submodule update --init assets/scenes/glTF-Sample-Models`
- For web builds: [Emscripten SDK](https://github.com/emscripten-core/emsdk)

## Getting Started

### macOS / Linux

**Desktop build:**
```sh
cd scripts/
./build.sh -CMakeToolchainFile ../third-party/vcpkg/scripts/buildsystems/vcpkg.cmake -Release
```

**With tests:**
```sh
cd scripts/
./build.sh -CMakeToolchainFile ../third-party/vcpkg/scripts/buildsystems/vcpkg.cmake -RunTests
```

**Web build:**
```sh
source <path-to-emsdk>/emsdk_env.sh
./build.sh -WebBuild
# Copy build artifacts (.js, .wasm, .data) to assets/web/html-js-wasm
# Start the nginx dev container in .devcontainer/nginx, copy artifacts to /usr/share/nginx/html
# Open http://localhost:8090/rotating_textured_cube.html or http://localhost:8090/opengl_viewer.html
```

**Packaging:**
```sh
cd build/
cpack -G DEB   # Linux — also TGZ, TXZ
cpack -G ZIP   # macOS — also TGZ
```

### Windows

**Desktop build:**
```powershell
cd scripts\
.\build.ps1 -CMakeToolchainFile ..\third-party\vcpkg\scripts\buildsystems\vcpkg.cmake -Release
```

**With tests:**
```powershell
cd scripts\
.\build.ps1 -CMakeToolchainFile ..\third-party\vcpkg\scripts\buildsystems\vcpkg.cmake -RunTests
```

**Packaging:**
```powershell
cd build\
cpack -G ZIP    # also NSIS, NSIS64
```

## Apps

| App | Description |
|-----|-------------|
| [opengl-viewer](./apps/opengl-viewer/README.md) | Full PBR viewer with IBL and ImGui settings panel |
| [rotating-textured-cube](./apps/rotating-textured-cube/README.md) | Minimal textured cube demo |
