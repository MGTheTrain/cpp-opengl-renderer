#!/bin/bash

set -euo pipefail

NoTests=true

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
    -h|--help)
      echo "Usage: $0 [-CMakeToolchainFile <path>] [-RunTests] [-h|--help]"
      echo "Options:"
      echo "  -CMakeToolchainFile   Path to CMAKE_TOOLCHAIN_FILE"
      echo "  -RunTests              Do not run tests"
      echo "  -h, --help            Display this help message"
      exit 0
      ;;
    *)
      echo "Unknown argument: $1"
      exit 1
      ;;
  esac
done

if [ -z "$CMakeToolchainFile" ]; then
  read -p "Enter the path to CMAKE_TOOLCHAIN_FILE: " CMakeToolchainFile
fi

currentDir=$(pwd)
cd "$currentDir/../../.."

BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m' 
echo "#####################################################################################################"
echo -e "$BLUE INFO: $NC About to compile the source code and link the binaries"

cmake -B build -DBUILD_LIB=ON -DBUILD_TEST=ON -DBUILD_APP=ON -DBUILD_PACKAGE=ON -DCMAKE_TOOLCHAIN_FILE="$CMakeToolchainFile" .
echo -e "$BLUE INFO: $NC Build environment could be successfully generated"

cmake --build build
echo -e "$BLUE INFO: $NC Compilation of the source code and linking binaries success"

if [ "$NoTests" = false ]; then
  cd "build"
  ctest --verbose
fi
cd "$currentDir"

echo -e "$BLUE INFO: $NC Success"