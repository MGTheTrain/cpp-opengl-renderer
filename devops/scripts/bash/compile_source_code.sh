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
    -RunTests)
      NoTests=false
      shift
      ;;
    -WebBuild)
      NoWebBuild=false
      shift
      ;;
    -h|--help)
      echo "Usage: $0 [-CMakeToolchainFile <path>] [-WebBuild] [-RunTests] [-h|--help]"
      echo "Options:"
      echo "  -CMakeToolchainFile          Path to CMAKE_TOOLCHAIN_FILE"
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

currentDir=$(pwd)
cd "$currentDir/../../.."

BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m' 
echo "#####################################################################################################"
echo -e "$BLUE INFO: $NC About to compile the source code and link the binaries"

if [[ "$NoWebBuild" == true ]]; then
  if [ -z "$CMakeToolchainFile" ]; then
    read -p "Enter the path to CMakeToolchainFile, e.g. /d/c++ repos/dependencies/vcpkg/scripts/buildsystems/vcpkg.cmake" CMakeToolchainFile
  fi
  cmake -B build -DBUILD_LIB=ON -DBUILD_TEST=ON -DBUILD_APP=ON -DBUILD_PACKAGE=ON -DCMAKE_TOOLCHAIN_FILE="$CMakeToolchainFile" .
  echo -e "$BLUE INFO: $NC Build environment could be successfully generated"

  cmake --build build --parallel 8
  echo -e "$BLUE INFO: $NC Compilation of the source code and linking binaries success"

  if [ "$NoTests" = false ]; then
    cd "build"
    ctest --verbose
  fi
else 
  # error: building glfw3:wasm32-emscripten failed with: BUILD_FAILED with vcpkg.json
  echo "Source the emscipten SDK, e.g. source <Path to emsdk folder>/emsdk_env.sh"
  mkdir -vp build
  cd build
  emcmake cmake -DBUILD_LIB=ON -DBUILD_TEST=ON -DBUILD_APP=ON -DBUILD_PACKAGE=ON -DBUILD_WEB=ON ..

  echo -e "$BLUE INFO: $NC Build environment could be successfully generated"

  make -j8
  echo -e "$BLUE INFO: $NC Compilation of the source code and linking binaries success"
fi

cd "$currentDir"

echo -e "$BLUE INFO: $NC Success"