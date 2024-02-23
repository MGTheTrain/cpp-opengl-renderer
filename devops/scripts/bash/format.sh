#!/bin/bash

SCRIPT_DIR=$(dirname "$BASH_SOURCE")
ROOT_PROJECT_DIR=$SCRIPT_DIR/../..

cd $ROOT_PROJECT_DIR

BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m' 

echo "#####################################################################################################"
echo -e "$BLUE INFO: $NC About to lint cpp files"
find . -name '*.h' -o -name '*.cpp' -exec clang-format -i {} \;

cd -