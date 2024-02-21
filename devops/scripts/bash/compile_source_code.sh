#!/bin/bash

set -euo pipefail

NoTests=false

while [ "$#" -gt 0 ]; do
  case "$1" in
    -CMakeToolchainFile)
      CMakeToolchainFile="$2"
      shift 2
      ;;
    -RunTests)
      NoTests=true
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
cmake -B build -DBUILD_LIB=ON -DBUILD_TEST=ON -DBUILD_APP=ON -DCMAKE_TOOLCHAIN_FILE="$CMakeToolchainFile" .

cmake --build build

if [ "$RunTests" = true ]; then
  cd "build"
  ctest
fi
cd "$currentDir"