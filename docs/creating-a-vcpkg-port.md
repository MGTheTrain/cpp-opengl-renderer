# Creating a vcpkg Port

Reference implementations:
- Case A: `third-party/vcpkg/ports/mgtt-tinyusdz` (upstream fork with install rules)
- Case B: `third-party/vcpkg/ports/tinyusdz` (manual glob fallback)

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
    { "name": "vcpkg-cmake", "host": true },
    { "name": "vcpkg-cmake-config", "host": true }
  ]
}
```

## 2. portfile.cmake

### Case A — upstream fork with CMake install() rules (preferred)

Use this when you control a fork of the upstream library and can add proper
`install()` rules. This is the approach used by `mgtt-tinyusdz`.

Add to the fork's `CMakeLists.txt` before calling `install(TARGETS ...)`:

```cmake
target_include_directories(${TINYUSDZ_TARGET_STATIC} PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include/tinyusdz>
)
install(TARGETS ${TINYUSDZ_TARGET_STATIC}
    EXPORT tinyusdzTargets
    ARCHIVE DESTINATION lib
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/
    DESTINATION include/tinyusdz
    FILES_MATCHING PATTERN "*.hh" PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.inc"
)
install(EXPORT tinyusdzTargets
    FILE tinyusdzConfig.cmake
    NAMESPACE tinyusdz::
    DESTINATION share/tinyusdz
)
```

Then the portfile is straightforward:

```cmake
if(VCPKG_TARGET_IS_EMSCRIPTEN)
    message(FATAL_ERROR "tinyusdz is not supported on Emscripten")
endif()

vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO MGTheTrain/tinyusdz
    REF v0.9.1
    SHA512 <hash>
    HEAD_REF main
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

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH share/tinyusdz PACKAGE_NAME tinyusdz)

file(REMOVE_RECURSE
    "${CURRENT_PACKAGES_DIR}/debug/include"
    "${CURRENT_PACKAGES_DIR}/include/tinyusdz/blender/sandbox"
    "${CURRENT_PACKAGES_DIR}/include/tinyusdz/external/glob/source"
    "${CURRENT_PACKAGES_DIR}/include/tinyusdz/external/nanobind/cmake"
    "${CURRENT_PACKAGES_DIR}/include/tinyusdz/external/nanobind/ext/robin_map/cmake"
    "${CURRENT_PACKAGES_DIR}/include/tinyusdz/external/nanobind/src/nanobind"
    "${CURRENT_PACKAGES_DIR}/include/tinyusdz/external/pybind11/tools"
    "${CURRENT_PACKAGES_DIR}/include/tinyusdz/osd/opensubdiv/tools/stringify"
)

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")

file(WRITE "${CURRENT_PACKAGES_DIR}/share/tinyusdz/usage" [[
tinyusdz provides CMake targets:

    find_package(tinyusdz CONFIG REQUIRED)
    target_link_libraries(main PRIVATE tinyusdz::tinyusdz_static)
]])
```

Linking `tinyusdz::tinyusdz_static` transitively propagates the include path
to all consumers — no `TINYUSDZ_INCLUDE_DIR` variable needed.

---

### Case B — no install() rules (manual glob fallback)

Use this when you cannot modify the upstream library. Headers and the static
library are copied manually from the build tree.

```cmake
vcpkg_cmake_build()

file(COPY "${SOURCE_PATH}/src/" DESTINATION "${CURRENT_PACKAGES_DIR}/include/tinyusdz"
    FILES_MATCHING
    PATTERN "*.hh" PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.inc"
)

if(VCPKG_TARGET_IS_WINDOWS)
    file(GLOB TINYUSDZ_LIB_REL "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-rel/*.lib")
    set(TINYUSDZ_LIB_FILENAME "tinyusdz_static.lib")
else()
    file(GLOB TINYUSDZ_LIB_REL "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-rel/*.a")
    set(TINYUSDZ_LIB_FILENAME "libtinyusdz_static.a")
endif()
file(COPY ${TINYUSDZ_LIB_REL} DESTINATION "${CURRENT_PACKAGES_DIR}/lib")

# Write a minimal config manually since there are no generated cmake files
file(WRITE "${CURRENT_PACKAGES_DIR}/share/tinyusdz/tinyusdzConfig.cmake" "
get_filename_component(_TINYUSDZ_PREFIX \"\${CMAKE_CURRENT_LIST_DIR}/../..\" ABSOLUTE)
set(TINYUSDZ_INCLUDE_DIR \"\${_TINYUSDZ_PREFIX}/include/tinyusdz\")

add_library(tinyusdz::tinyusdz_static STATIC IMPORTED)
set_target_properties(tinyusdz::tinyusdz_static PROPERTIES
    IMPORTED_LOCATION \"\${_TINYUSDZ_PREFIX}/lib/${TINYUSDZ_LIB_FILENAME}\"
    INTERFACE_INCLUDE_DIRECTORIES \"\${_TINYUSDZ_PREFIX}/include/tinyusdz\"
)
")
```

Note: Case B requires `TINYUSDZ_INCLUDE_DIR` alongside
`target_link_libraries` since `INTERFACE_INCLUDE_DIRECTORIES` may not
propagate reliably without a proper exported target.

## 3. Version files

**`versions/baseline.json`:**
```json
"tinyusdz": { "baseline": "v0.9.1", "port-version": 0 }
```

**`versions/t-/tinyusdz.json`:**
```json
{ "versions": [{ "version-string": "v0.9.1", "git-tree": "" }] }
```

## 4. Get the SHA512

Set `SHA512 0` first, run the build — vcpkg prints the correct hash in the
error output, then paste it in.

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
"dependencies": ["mgtt-tinyusdz"]
```

**`CMakeLists.txt`** (Case A — link only, includes propagate transitively):
```cmake
find_package(tinyusdz CONFIG QUIET)
if(tinyusdz_FOUND AND NOT BUILD_WEB)
    # PUBLIC so consumers of this static lib inherit headers and the library
    target_link_libraries(${TARGET} PUBLIC tinyusdz::tinyusdz_static)
endif()
```

## Key gotchas

- Prefer Case A — proper `install()` rules give correct `INTERFACE_INCLUDE_DIRECTORIES`
  on the exported target, so linking alone is sufficient for all consumers
- With Case A and `PUBLIC` linkage on a static lib, downstream targets inherit
  tinyusdz headers transitively — no duplicate `target_link_libraries` needed
- `TARGET_TRIPLET` — use this instead of hardcoding triplets in lib globs (Case B)
- Always run `format-manifest` and `x-add-version` before pushing to a registry fork
