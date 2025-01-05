# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Updated

- Utilized fixed size unsigned and signed integers in modules
- Replaced helper method in `gltf-scene-importer` with initalizer_list helper function in separate header file in modules
- Considered passing default data types by value, objects by reference in modules
- Avoided setting empty strings during construction in modules

## [0.8.0] - 05-01-2024

### Added

- Added vcpkg git submodule 
- Added pull request and issue templates

### Updated

- Replaced deprecated nativefiledialog with nativefiledialog-extended package and adjusted implementation in OpenGL viewer app
- Moved texture manager declarations and implementations in separate header and source files

## [0.7.0] - 17-03-2024

### Added

- [Feature] Add nginx web server dev container
- [Feature] Port Desktop application to the web trough emscripten SDK

### Updated

- Update `CMakeLists.txt` to support web builds. Update bash and ps1 script to compile C++ code trough the `emscripten SDK` for rendering in web browsers

### Fixed

- [Fix] Reset AABB attributes for newly loaded scenes
- [Fix] Add vcpkg to dev containers with tools for C++ repos

## [0.6.0] - 14-03-2024

### Added

- [Feature] CI workflows ensuring continuous testing of the modules. Test executables rely on `glfw3` and `glew`, requiring a graphical environment. On Linux Ubuntu OS, a virtual framebuffer is established using `xvfb` for those tests. However, for MacOS and Windows OS, only builds are feasible due to the complexities or impossibility of replicating similar setups.

## [0.5.0] - 13-03-2024

### Added

- [Feature] Native file dialog support for loading new glTF scenes

### Updated

- Introduce helper methods within the `GltfSceneImporter` class to facilitate the correct initial transformations of the scene and its node meshes. These transformations are essential for initially centering the model according to the scene's AABB.

### Fixed

- [Fix] Ensure only node meshes are cleared when using the `Clear()` method in [node.cpp](modules/rendering/src/model/node.cpp)

## [0.4.0] - 12-03-2024

### Added

- [Feature] Integrated simple menu (e.g. with sliders and other widgets)

## [0.3.0] - 27-02-2024

### Added

- [Feature] Cross-platform CPack packaging for Linux OSes, MacOS and Windows OSes

## [0.2.0] - 23-02-2024

### Added

- [Feature] Add git submodule providing several glTF samples to load and render
- [Feature] Create `CMakeLists.txt` files for cross-platform build environment generation
- [Feature] High code coverage with GoogleTest for methods in rendering and window module structs and classes 
- [Feature] CI workflows ensuring cross-platform compilation primarily for Desktop operating systems
- [Feature] CI workflows ensuring a coherent code base trough auto-formatting with clang-format and security with cpplint 

## [0.1.0] - 17-02-2024

### Added

- [Feature] Support for glTF scene loading
- [Feature] PBR shader pipeline considering primarily the metal roughness workflow