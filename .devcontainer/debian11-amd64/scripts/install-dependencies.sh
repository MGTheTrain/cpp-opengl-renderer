set -euo pipefail

apt-get update
apt-get install -y libxmu-dev libxi-dev libgl-dev \
        libxinerama-dev libxcursor-dev xorg-dev \
        libglu1-mesa-dev pkg-config python3 python3-pip
pip3 install clang-format cpplint

cd tmp
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest-upstream
./emsdk activate latest-upstream
