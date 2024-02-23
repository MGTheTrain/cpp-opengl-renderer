set -euo pipefail

apt-get update
apt-get install -y python3 python3-pip
pip3 install clang-format cpplint