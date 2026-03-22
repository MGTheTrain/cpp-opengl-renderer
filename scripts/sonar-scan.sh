#!/usr/bin/env bash

# scripts/sonar-scan.sh
# Runs SonarCloud analysis locally using compilation database (ARM64 compatible).
# Prerequisites:
#   brew install sonar-scanner
#   export SONAR_TOKEN=<your-token>
# Usage: ./scripts/sonar-scan.sh

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${REPO_ROOT}/build"

if [ -z "${SONAR_TOKEN:-}" ]; then
  echo "Error: SONAR_TOKEN is not set. Export it before running this script."
  exit 1
fi

if ! command -v sonar-scanner &> /dev/null; then
  echo "Error: sonar-scanner not found. Install it with: brew install sonar-scanner"
  exit 1
fi

echo "Configuring with compile_commands.json..."
cmake -S "${REPO_ROOT}" -B "${BUILD_DIR}" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DBUILD_LIB=ON -DBUILD_APP=ON -DBUILD_TEST=OFF \
  -DCMAKE_TOOLCHAIN_FILE="${REPO_ROOT}/third-party/vcpkg/scripts/buildsystems/vcpkg.cmake"

echo "Building..."
cmake --build "${BUILD_DIR}"

echo "Running SonarCloud analysis..."
cd "${REPO_ROOT}"
sonar-scanner \
  -Dsonar.token="${SONAR_TOKEN}" \
  -Dsonar.cfamily.compile-commands="${BUILD_DIR}/compile_commands.json"

echo "Done. Check https://sonarcloud.io for results."
