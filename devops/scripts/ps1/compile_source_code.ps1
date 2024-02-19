cd ../..
cmake -B build -DBUILD_LIB=ON -DBUILD_TEST=ON  -DCMAKE_TOOLCHAIN_FILE="D:\c++ repos\dependencies\vcpkg\scripts\buildsystems\vcpkg.cmake"  .
cmake --build build