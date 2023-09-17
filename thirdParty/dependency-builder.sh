#!/usr/bin/bash

printf "Current working dir should be inside thirdParty, confirm it: \033[0;31m$(pwd)\n\033[0mIf it's not, use ctrl+c to stop script:\n"
read -p "Enter to continue or ctrl+c to stop:    "

[ ! -e absl/ ] || rm -rf absl/
[ ! -e re2/ ] || rm -rf re2

mkdir sources
cd sources
git clone --progress https://github.com/abseil/abseil-cpp.git
git clone --progress https://github.com/google/re2.git

clear

cd abseil-cpp
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=../../../absl
cmake --build . --target install


clear

cd ../../
cd re2
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=../../../re2 -Dabsl_DIR=../../../absl/lib/cmake/absl/
cmake --build . --target install

clear

cd ../../../
rm -rf sources
