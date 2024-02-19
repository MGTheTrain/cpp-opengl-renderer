#!/bin/bash

set -euo pipefail

while [ "$#" -gt 0 ]; do
  case "$1" in
    -CMakeToolchainFile)
      CMakeToolchainFile="$2"
      shift 2
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
echo "./compile_source_code.sh -CMakeToolchainFile \"$CMakeToolchainFile\""
cmake -B build -DBUILD_LIB=ON -DBUILD_TEST=ON -DBUILD_APP=ON -DCMAKE_TOOLCHAIN_FILE="$CMakeToolchainFile" .
cmake --build build
cd "$currentDir"
