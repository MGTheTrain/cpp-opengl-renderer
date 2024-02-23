#!/bin/bash

SCRIPT_DIR=$(dirname "$BASH_SOURCE")
ROOT_PROJECT_DIR=$SCRIPT_DIR/../../..

cd $ROOT_PROJECT_DIR

BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m' 

echo "#####################################################################################################"
echo -e "$BLUE INFO: $NC About to format and lint cpp files"
find ./apps/** -name '*.h' -o -name '*.cpp' | xargs clang-format -style=Google -i
find ./modules/** -name '*.h' -o -name '*.cpp' | xargs clang-format -style=Google -i

# The linter and auto-formatter are not in alignment, requiring local resolution of linter findings.
# It's worth noting that `cpplint` int the subsequent lines doesn't produce an exit code greater than 0 here even when there are issues. 
find ./apps/** -name '*.h' -o -name '*.cpp' | xargs cpplint
find ./modules/** -name '*.h' -o -name '*.cpp' | xargs cpplint

cd -