#!/usr/bin/env bash
# scripts/test-naming-conventions.sh
# Validates that .clang-tidy naming convention rules fire correctly.
# Usage: ./scripts/test-naming-conventions.sh

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
TEST_FILE="/tmp/naming_convention_test.cpp"
COMPILE_DB="/tmp/compile_commands.json"

cat > "${TEST_FILE}" << 'EOF'
// Naming convention violation test — every bad name below should be flagged.

struct my_struct {        // bad: StructCase must be PascalCase
  int PublicField{0};
};

class my_class {          // bad: ClassCase must be PascalCase
 private:
  int member{0};          // bad: private member must have trailing underscore
  int member_{0};         // good
};

void my_function() {      // bad: FunctionCase must be PascalCase
  int MyLocal = 0;        // bad: LocalVariableCase must be camelBack
  int goodLocal = 0;      // good
  static_cast<void>(MyLocal);
  static_cast<void>(goodLocal);
}

void GoodFunction(int MyParam) {   // bad: ParameterCase must be camelBack
  static_cast<void>(MyParam);
}

constexpr int BAD_CONSTANT = 1;    // bad: must be kPascalCase
constexpr int kGoodConstant = 1;   // good
static_cast<void>(BAD_CONSTANT);
static_cast<void>(kGoodConstant);
EOF

echo "[{\"directory\":\"/tmp\",\"command\":\"clang++ -std=c++17 ${TEST_FILE}\",\"file\":\"${TEST_FILE}\"}]" \
  > "${COMPILE_DB}"

echo "Running clang-tidy naming convention checks..."

EXPECTED_VIOLATIONS=(
  "my_struct"
  "my_class"
  "member"
  "my_function"
  "MyLocal"
  "MyParam"
  "BAD_CONSTANT"
)

# clang-tidy "${TEST_FILE}" \
#   -p /tmp \
#   --config-file="${REPO_ROOT}/.clang-tidy" \
#   --checks='readability-identifier-naming'

OUTPUT=$(clang-tidy "${TEST_FILE}" \
  -p /tmp \
  --config-file="${REPO_ROOT}/.clang-tidy" \
  --checks='readability-identifier-naming' 2>&1 || true)

FAILED=0
for violation in "${EXPECTED_VIOLATIONS[@]}"; do
  if echo "${OUTPUT}" | grep -q "${violation}"; then
    echo "  [PASS] '${violation}' was correctly flagged"
  else
    echo "  [FAIL] '${violation}' was NOT flagged — check CheckOptions"
    FAILED=1
  fi
done

rm -f "${TEST_FILE}" "${COMPILE_DB}"

if [ "${FAILED}" -eq 0 ]; then
  echo ""
  echo "All naming convention violations detected correctly."
  exit 0
else
  echo ""
  echo "One or more naming convention rules did not fire. Review .clang-tidy CheckOptions."
  exit 1
fi
