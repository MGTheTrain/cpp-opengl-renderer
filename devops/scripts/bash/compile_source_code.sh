#!/bin/bash

set -euo pipefail

NoTests=true
NoWebBuild=true

while [ "$#" -gt 0 ]; do
  case "$1" in
    -CMakeToolchainFile)
      CMakeToolchainFile="$2"
      shift 2
      ;;
    -EmscriptenCMakeFile)
      EmscriptenCMakeFile="$3"
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
    -h|--help)
      echo "Usage: $0 [-CMakeToolchainFile <path>] [-EmscriptenCMakeFile <path>] [-WebBuild] [-RunTests] [-h|--help]"
      echo "Options:"
      echo "  -CMakeToolchainFile          Path to CMAKE_TOOLCHAIN_FILE"
      echo "  -EmscriptenCMakeFile <Path>  Path to EMSCRIPTEN_CMAKE_FILE"
      echo "  -RunTests                    Run tests after building (optional)"
      echo "  -WebBuild                    Build for web (with emscripten) instead of Desktop (optional)"
      echo "  -h, --help                   Display this help message"
      exit 0
      ;;
    *)
      echo "Unknown argument: $1"
      exit 1
      ;;
  esac
done

if [ -z "$CMakeToolchainFile" ]; then
  read -p "Enter the path to CMakeToolchainFile, e.g. /d/c++ repos/dependencies/vcpkg/scripts/buildsystems/vcpkg.cmake" CMakeToolchainFile
fi

currentDir=$(pwd)
cd "$currentDir/../../.."

BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m' 
echo "#####################################################################################################"
echo -e "$BLUE INFO: $NC About to compile the source code and link the binaries"

if [[ "$NoWebBuild" == true ]]; then
  cmake -B build -DBUILD_LIB=ON -DBUILD_TEST=ON -DBUILD_APP=ON -DBUILD_PACKAGE=ON -DCMAKE_TOOLCHAIN_FILE="$CMakeToolchainFile" .
else 
  if [ -z "$EmscriptenCMakeFile" ]; then
    read -p "Enter the path to EmscriptenCMakeFile, e.g. /d/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" EmscriptenCMakeFile
  fi
  cmake -B build -DBUILD_LIB=ON -DBUILD_TEST=ON -DBUILD_APP=ON -DBUILD_PACKAGE=ON -DCMAKE_TOOLCHAIN_FILE="$CMakeToolchainFile" -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE="$EmscriptenCMakeFile" -DVCPKG_TARGET_TRIPLET="wasm32-emscripten" .
fi

echo -e "$BLUE INFO: $NC Build environment could be successfully generated"

cmake --build build
echo -e "$BLUE INFO: $NC Compilation of the source code and linking binaries success"

if [ "$NoTests" = false ]; then
  cd "build"
  ctest --verbose
fi
cd "$currentDir"

echo -e "$BLUE INFO: $NC Success"