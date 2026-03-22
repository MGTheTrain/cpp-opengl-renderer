#!/usr/bin/env bash

# scripts/sonar-scan.sh
# Runs SonarCloud analysis locally using compilation database (ARM64 compatible).
# Prerequisites:
#   brew install sonar-scanner lcov
#   pip install gcovr
#   export SONAR_TOKEN=<your-token>
# Usage: ./scripts/sonar-scan.sh

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${REPO_ROOT}/build"
COVERAGE_DIR="${REPO_ROOT}/coverage"
PROJECT_KEY="MGTheTrain_cpp-opengl-renderer"

if [ -z "${SONAR_TOKEN:-}" ]; then
  echo "Error: SONAR_TOKEN is not set. Export it before running this script."
  exit 1
fi

if ! command -v sonar-scanner &> /dev/null; then
  echo "Error: sonar-scanner not found. Install it with: brew install sonar-scanner"
  exit 1
fi

if ! command -v lcov &> /dev/null; then
  echo "Error: lcov not found. Install it with: brew install lcov"
  exit 1
fi

if ! command -v gcovr &> /dev/null; then
  echo "Error: gcovr not found. Install it with: pip install gcovr"
  exit 1
fi

echo "Configuring with coverage and compile_commands.json..."
cmake -S "${REPO_ROOT}" -B "${BUILD_DIR}" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DBUILD_LIB=ON -DBUILD_APP=OFF -DBUILD_TEST=ON \
  -DCMAKE_TOOLCHAIN_FILE="${REPO_ROOT}/third-party/vcpkg/scripts/buildsystems/vcpkg.cmake"

echo "Building..."
cmake --build "${BUILD_DIR}"

echo "Running tests..."
cd "${BUILD_DIR}"
ctest --output-on-failure
cd "${REPO_ROOT}"

echo "Generating coverage report..."
mkdir -p "${COVERAGE_DIR}"
lcov --capture \
  --directory "${BUILD_DIR}" \
  --output-file "${COVERAGE_DIR}/lcov.info" \
  --exclude '*/test/*' \
  --exclude '/usr/*' \
  --exclude '*/vcpkg_installed/*' \
  --ignore-errors inconsistent,unsupported,unused

echo "Converting coverage to SonarQube XML..."
gcovr \
  --sonarqube "${COVERAGE_DIR}/sonarqube.xml" \
  --root "${REPO_ROOT}" \
  --filter 'modules/' \
  --exclude '.*test.*' \
  --exclude '.*vcpkg_installed.*' \
  "${BUILD_DIR}"

echo "Running SonarCloud analysis..."
sonar-scanner \
  -Dsonar.token="${SONAR_TOKEN}" \
  -Dsonar.cfamily.compile-commands="${BUILD_DIR}/compile_commands.json" \
  -Dsonar.coverageReportPaths="${COVERAGE_DIR}/sonarqube.xml"

echo "Waiting for SonarCloud to process analysis..."
sleep 5

echo "Checking quality thresholds..."
RESPONSE=$(curl -s \
  "https://sonarcloud.io/api/measures/component?component=${PROJECT_KEY}&metricKeys=coverage,reliability_rating,security_rating,sqale_rating")

python3 -c "
import sys, json

data = json.loads('${RESPONSE}')
measures = {m['metric']: m['value'] for m in data['component']['measures']}

coverage = float(measures['coverage'])
reliability = int(float(measures['reliability_rating']))
security = int(float(measures['security_rating']))
maintainability = int(float(measures['sqale_rating']))

failed = False
if coverage < 65.0:
    print(f'FAIL: Coverage {coverage}% is below 65%')
    failed = True
if reliability != 1:
    print(f'FAIL: Reliability rating is not A (got {reliability})')
    failed = True
if security != 1:
    print(f'FAIL: Security rating is not A (got {security})')
    failed = True
if maintainability != 1:
    print(f'FAIL: Maintainability rating is not A (got {maintainability})')
    failed = True

if failed:
    sys.exit(1)
print(f'All checks passed — Coverage: {coverage}%, Reliability: A, Security: A, Maintainability: A')
"

echo "Done. Check https://sonarcloud.io for results."
