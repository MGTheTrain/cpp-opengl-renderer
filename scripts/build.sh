#!/bin/bash

set -euo pipefail

NoTests=true
NoWebBuild=true
BuildType="Debug"
RevisionNumber=""

while [ "$#" -gt 0 ]; do
  case "$1" in
    -CMakeToolchainFile)
      CMakeToolchainFile="$2"
      shift 2
      ;;
    -RunTests)
      NoTests=false
      shift
      ;;
    -WebBuild)
      NoWebBuild=false
      shift
      ;;
    -Release)
      BuildType="Release"
      shift
      ;;
    -RevisionNumber)
      RevisionNumber="$2"
      shift 2
      ;;
    -h|--help)
      echo "Usage: $0 [-CMakeToolchainFile <path>] [-WebBuild] [-RunTests] [-Release] [-RevisionNumber <rev>] [-h|--help]"
      exit 0
      ;;
    *)
      echo "Unknown argument: $1"
      exit 1
      ;;
  esac
done

currentDir=$(pwd)
cd "$currentDir/.."

BLUE='\033[0;34m'
NC='\033[0m'

REVISION_ARG=""
if [ -n "$RevisionNumber" ]; then
  REVISION_ARG="-DREVISION_NUMBER=${RevisionNumber}"
fi

if [[ "$NoWebBuild" == true ]]; then
  if [ -z "${CMakeToolchainFile:-}" ]; then
    read -p "Enter the path to CMakeToolchainFile: " CMakeToolchainFile
  fi

  if [ "$NoTests" = false ]; then
    echo -e "$BLUE INFO: $NC Configuring combined test + app build (Debug)"
    cmake -B build \
      -DBUILD_LIB=ON -DBUILD_TEST=ON -DBUILD_APP=ON -DBUILD_PACKAGE=ON \
      -DCMAKE_BUILD_TYPE="${BuildType}" \
      -DCMAKE_TOOLCHAIN_FILE="$CMakeToolchainFile" \
      ${REVISION_ARG} \
      .
    cmake --build build --parallel --config "${BuildType}"
    echo -e "$BLUE INFO: $NC Running tests"
    cd build && ctest -C "${BuildType}" --verbose && cd ..
  else
    echo -e "$BLUE INFO: $NC Configuring app-only build (${BuildType})"
    cmake -B build \
      -DBUILD_LIB=ON -DBUILD_TEST=OFF -DBUILD_APP=ON -DBUILD_PACKAGE=ON \
      -DCMAKE_BUILD_TYPE="${BuildType}" \
      -DCMAKE_TOOLCHAIN_FILE="$CMakeToolchainFile" \
      ${REVISION_ARG} \
      .
    cmake --build build --parallel --config "${BuildType}"
  fi

  echo -e "$BLUE INFO: $NC Build (${BuildType}) complete"

else
  echo "Source the emscripten SDK first, e.g. source <path>/emsdk_env.sh"
  echo "Note: tinyusdz will be pre-built and installed for Emscripten"

  TINYUSDZ_SRC=${currentDir}/../assets/web/third-party/tinyusdz
  TINYUSDZ_INSTALL=${currentDir}/../assets/web/third-party/tinyusdz-install

  if [ ! -f "${TINYUSDZ_INSTALL}/share/tinyusdz/tinyusdzConfig.cmake" ]; then
    echo -e "$BLUE INFO: $NC Pre-building tinyusdz for Emscripten"
    mkdir -p "${TINYUSDZ_INSTALL}"
    emcmake cmake -S "${TINYUSDZ_SRC}" -B "${TINYUSDZ_SRC}/build-wasm" \
      -DCMAKE_BUILD_TYPE=Release \
      -DTINYUSDZ_BUILD_TESTS=OFF \
      -DTINYUSDZ_BUILD_EXAMPLES=OFF \
      -DTINYUSDZ_BUILD_BENCHMARKS=OFF \
      -DTINYUSDZ_WITH_OPENSUBDIV=OFF \
      -DTINYUSDZ_WITH_EXR=OFF \
      -DTINYUSDZ_WITH_AUDIO=OFF \
      -DTINYUSDZ_WITH_USDMTLX=OFF \
      -DTINYUSDZ_WITH_PXR_COMPAT_API=OFF \
      -DTINYUSDZ_PRODUCTION_BUILD=ON \
      -DBUILD_SHARED_LIBS=OFF \
      -DCMAKE_INSTALL_PREFIX="${TINYUSDZ_INSTALL}"
    cmake --build "${TINYUSDZ_SRC}/build-wasm" --parallel
    cmake --install "${TINYUSDZ_SRC}/build-wasm"
    echo -e "$BLUE INFO: $NC tinyusdz pre-build complete"
  else
    echo -e "$BLUE INFO: $NC tinyusdz already pre-built, skipping"
  fi

  mkdir -vp build
  cd build
  emcmake cmake \
    -DBUILD_LIB=ON -DBUILD_TEST=OFF -DBUILD_APP=ON -DBUILD_PACKAGE=ON -DBUILD_WEB=ON \
    -DTINYUSDZ_INSTALL_PREFIX="${TINYUSDZ_INSTALL}" \
    ${REVISION_ARG} \
    ..
  make -j8
  echo -e "$BLUE INFO: $NC Web build complete"
  cd ..
fi

cd "$currentDir"
echo -e "$BLUE INFO: $NC Success"
