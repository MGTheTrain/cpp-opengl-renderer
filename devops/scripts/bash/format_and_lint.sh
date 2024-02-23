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
find ./apps/** -name '*.h' -o -name '*.cpp' | xargs cpplint
find ./modules/** -name '*.h' -o -name '*.cpp' | xargs cpplint

cd -