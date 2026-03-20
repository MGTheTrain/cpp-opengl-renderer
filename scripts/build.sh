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
      echo "Options:"
      echo "  -CMakeToolchainFile          Path to CMAKE_TOOLCHAIN_FILE"
      echo "  -RunTests                    Run tests after building (always uses Debug)"
      echo "  -WebBuild                    Build for web (with emscripten) instead of Desktop"
      echo "  -Release                     Build in Release mode (default: Debug)"
      echo "  -RevisionNumber              Revision suffix appended to package version (e.g. alpha-42)"
      echo "  -h, --help                   Display this help message"
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
echo "#####################################################################################################"
echo -e "$BLUE INFO: $NC About to compile the source code and link the binaries"

if [[ "$NoWebBuild" == true ]]; then
  if [ -z "${CMakeToolchainFile:-}" ]; then
    read -p "Enter the path to CMakeToolchainFile: " CMakeToolchainFile
  fi

  REVISION_ARG=""
  if [ -n "$RevisionNumber" ]; then
    REVISION_ARG="-DREVISION_NUMBER=${RevisionNumber}"
  fi

  # Tests always build in Debug regardless of -Release flag
  if [ "$NoTests" = false ]; then
    echo -e "$BLUE INFO: $NC Configuring test build (Debug)"
    cmake -B build-test \
      -DBUILD_LIB=ON -DBUILD_TEST=ON -DBUILD_APP=OFF -DBUILD_PACKAGE=OFF \
      -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_TOOLCHAIN_FILE="$CMakeToolchainFile" \
      .
    cmake --build build-test --parallel --config Debug
    echo -e "$BLUE INFO: $NC Running tests"
    cd build-test && ctest -C Debug --verbose && cd ..
  fi

  echo -e "$BLUE INFO: $NC Configuring app build (${BuildType})"
  cmake -B build \
    -DBUILD_LIB=ON -DBUILD_TEST=OFF -DBUILD_APP=ON -DBUILD_PACKAGE=ON \
    -DCMAKE_BUILD_TYPE="${BuildType}" \
    -DCMAKE_TOOLCHAIN_FILE="$CMakeToolchainFile" \
    ${REVISION_ARG} \
    .
  cmake --build build --parallel --config "${BuildType}"
  echo -e "$BLUE INFO: $NC Build (${BuildType}) complete"

else
  echo "Source the emscripten SDK first, e.g. source <path>/emsdk_env.sh"
  mkdir -vp build
  cd build

  REVISION_ARG=""
  if [ -n "$RevisionNumber" ]; then
    REVISION_ARG="-DREVISION_NUMBER=${RevisionNumber}"
  fi

  emcmake cmake \
    -DBUILD_LIB=ON -DBUILD_TEST=OFF -DBUILD_APP=ON -DBUILD_PACKAGE=ON -DBUILD_WEB=ON \
    ${REVISION_ARG} \
    ..
  make -j8
  echo -e "$BLUE INFO: $NC Web build complete"
fi

cd "$currentDir"
echo -e "$BLUE INFO: $NC Success"
