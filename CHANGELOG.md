# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [0.5.0] - 13-04-2024

### Added

- [Feature] Native file dialog support for loading new glTF scenes

### Updated

- Introduce helper methods within the `GltfSceneImporter` class to facilitate the correct initial transformations of the scene and its node meshes. These transformations are essential for initially centering the model according to the scene's AABB.

### Fixed

- [Fix] Ensure only node meshes are cleared when using the `Clear()` method in [node.cpp](modules/rendering/src/model/node.cpp)

## [0.4.0] - 12-04-2024

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