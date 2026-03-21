set -euo pipefail

apt-get update
apt-get install -y \
    build-essential cmake \
    clang clang-format clang-tidy \
    libxmu-dev libxi-dev libgl-dev \
    libxinerama-dev libxcursor-dev xorg-dev \
    libglu1-mesa-dev pkg-config \
    autoconf autoconf-archive automake libtool libltdl-dev \
    python3 python3-pip

pip3 install cpplint --break-system-packages

cd /tmp/
# vcpkg
rm -rf vcpkg
git clone https://github.com/Microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh
# emscripten SDK
rm -rf emsdk
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest-upstream
./emsdk activate latest-upstream
