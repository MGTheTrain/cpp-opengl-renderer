
# Creating a vcpkg Port
Reference implementation: `third-party/vcpkg/ports/tinyusdz`

## Directory structure

```sh
ports/<name>/
├── portfile.cmake   # build instructions
└── vcpkg.json       # metadata and dependencies
versions/
├── baseline.json    # latest version per port
└── <first-letter>-/<name>.json  # version history
```

## 1. vcpkg.json

```json
{
  "name": "tinyusdz",
  "version-string": "v0.9.1",
  "description": "Tiny USDZ/USD loader and writer library",
  "homepage": "https://github.com/lighttransport/tinyusdz",
  "license": "MIT",
  "dependencies": [
    {
      "name": "vcpkg-cmake",
      "host": true
    },
    {
      "name": "vcpkg-cmake-config",
      "host": true
    }
  ]
}
```

## 2. portfile.cmake

Two cases:

**A — library has CMake install() rules** (simple):
```cmake
vcpkg_from_github(OUT_SOURCE_PATH SOURCE_PATH REPO org/lib REF v1.0 SHA512 <hash> HEAD_REF main)
vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}" OPTIONS -DBUILD_TESTS=OFF)
vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/<name>)
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
```

**B — library has no install() rules** (tinyusdz case — manual):
```cmake
if(VCPKG_TARGET_IS_EMSCRIPTEN)
    message(FATAL_ERROR "tinyusdz is not supported on Emscripten")
endif()

vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO lighttransport/tinyusdz
    REF v0.9.1
    SHA512 26093ec107e1440be1e896ba3da8e0d9196c968455332d1d6961cbe458a26cce86d18f4b22279b5775a5e029e491306346aa4796517fac7e30a6ba1ff84d2e71
    HEAD_REF dev
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DTINYUSDZ_BUILD_TESTS=OFF
        -DTINYUSDZ_BUILD_EXAMPLES=OFF
        -DTINYUSDZ_BUILD_BENCHMARKS=OFF
        -DTINYUSDZ_WITH_OPENSUBDIV=OFF
        -DTINYUSDZ_WITH_EXR=OFF
        -DTINYUSDZ_WITH_AUDIO=OFF
        -DTINYUSDZ_WITH_USDMTLX=OFF
        -DTINYUSDZ_WITH_PXR_COMPAT_API=OFF
        -DBUILD_SHARED_LIBS=OFF
)

vcpkg_cmake_build()

file(COPY "${SOURCE_PATH}/src/" DESTINATION "${CURRENT_PACKAGES_DIR}/include/tinyusdz"
    FILES_MATCHING
    PATTERN "*.hh"
    PATTERN "*.h"
    PATTERN "*.hpp"
    PATTERN "*.inc"
)

# Release lib
if(VCPKG_TARGET_IS_WINDOWS)
    file(GLOB TINYUSDZ_LIB_REL
        "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-rel/*.lib"
        "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-rel/src/*.lib"
    )
    set(TINYUSDZ_LIB_FILENAME "tinyusdz_static.lib")
else()
    file(GLOB TINYUSDZ_LIB_REL
        "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-rel/*.a"
        "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-rel/src/*.a"
        "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-rel/lib/*.a"
    )
    set(TINYUSDZ_LIB_FILENAME "libtinyusdz_static.a")
endif()
file(COPY ${TINYUSDZ_LIB_REL} DESTINATION "${CURRENT_PACKAGES_DIR}/lib")

# Debug lib
if(VCPKG_TARGET_IS_WINDOWS)
    file(GLOB TINYUSDZ_LIB_DBG
        "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-dbg/*.lib"
        "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-dbg/src/*.lib"
    )
else()
    file(GLOB TINYUSDZ_LIB_DBG
        "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-dbg/*.a"
        "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-dbg/src/*.a"
        "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-dbg/lib/*.a"
    )
endif()
if(TINYUSDZ_LIB_DBG)
    file(COPY ${TINYUSDZ_LIB_DBG} DESTINATION "${CURRENT_PACKAGES_DIR}/debug/lib")
endif()

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")

# Expose include dir as a variable — same pattern as stb/tinygltf
file(WRITE "${CURRENT_PACKAGES_DIR}/share/tinyusdz/tinyusdzConfig.cmake" "
get_filename_component(_TINYUSDZ_PREFIX \"\${CMAKE_CURRENT_LIST_DIR}/../..\" ABSOLUTE)
set(TINYUSDZ_INCLUDE_DIR \"\${_TINYUSDZ_PREFIX}/include/tinyusdz\")

add_library(tinyusdz::tinyusdz_static STATIC IMPORTED)
set_target_properties(tinyusdz::tinyusdz_static PROPERTIES
    IMPORTED_LOCATION \"\${_TINYUSDZ_PREFIX}/lib/${TINYUSDZ_LIB_FILENAME}\"
    IMPORTED_LOCATION_DEBUG \"\${_TINYUSDZ_PREFIX}/debug/lib/${TINYUSDZ_LIB_FILENAME}\"
    INTERFACE_INCLUDE_DIRECTORIES \"\${_TINYUSDZ_PREFIX}/include/tinyusdz\"
)
")
```

## 3. Version files

**`versions/baseline.json`** — add entry:
```json
"tinyusdz": { "baseline": "v0.9.1", "port-version": 0 }
```

**`versions/t-/tinyusdz.json`** — git-tree is filled by tooling:
```json
{ "versions": [{ "version-string": "v0.9.1", "git-tree": "" }] }
```

## 4. Get the SHA512

Set `SHA512 0` first, run the build — vcpkg prints the correct hash in the error, paste it in.

## 5. Register the version

```bash
./vcpkg format-manifest ports/tinyusdz/vcpkg.json
./vcpkg x-add-version --all --overwrite-version
git add ports/tinyusdz/ versions/
git commit -m "[tinyusdz] add v0.9.1 vcpkg port with manual header and static library install"
```

## 6. Consume from your project

**`vcpkg.json`:**
```json
...
"dependencies": [
        ...
        "tinyusdz",
...
```

**`CMakeLists.txt`:**
```cmake
find_package(tinyusdz CONFIG QUIET)
if(tinyusdz_FOUND)
    target_link_libraries(${TARGET} PRIVATE tinyusdz::tinyusdz_static)
endif()
```

## Key gotchas

- `TARGET_TRIPLET` — use this instead of hardcoding `arm64-osx` in lib globs
- `VCPKG_TARGET_IS_WINDOWS` / `VCPKG_TARGET_IS_EMSCRIPTEN` — guard platform-specific logic
- Static libs with coverage flags (`--coverage`) must not be linked into non-test executables — apply coverage only to test executable targets
- Always run `format-manifest` and `x-add-version` before pushing to a registry fork
