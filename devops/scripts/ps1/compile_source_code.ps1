param(
    [string]$CMakeToolchainFile,
    [switch]$RunTests,
    [switch]$Help,
    [switch]$WebBuild
)

function Show-Help {
    echo "Usage: .\compile_source_code.ps1 -CMakeToolchainFile <Path> -EmscriptenCMakeFile <Path> [-RunTests] [-WebBuild] [-Help]"
    echo "  -CMakeToolchainFile <Path>   Path to CMAKE_TOOLCHAIN_FILE"
    echo "  -EmscriptenCMakeFile <Path>  Path to EMSCRIPTEN_CMAKE_FILE"
    echo "  -RunTests                    Run tests after building (optional)"
    echo "  -WebBuild                    Build for web (with emscripten) instead of Desktop (optional)"
    echo "  -Help                        Display this help message"
}

if ($Help) {
    Show-Help
    exit
}

if (-not $CMakeToolchainFile) {
    $CMakeToolchainFile = Read-Host "Enter the path to CMakeToolchainFile, e.g. 'D:\c++ repos\dependencies\vcpkg\scripts\buildsystems\vcpkg.cmake'"
}

$currentDir = Get-Location
Set-Location -Path "$PSScriptRoot/../../.."

if ($WebBuild) {
    if (-not $EmscriptenCMakeFile) {
        $EmscriptenCMakeFile = Read-Host "Enter the path to EmscriptenCMakeFile, e.g. 'D:\emsdk\upstream\emscripten\cmake\Modules\Platform\Emscripten.cmake'"
    }
    cmake -B build -DBUILD_LIB=ON -DBUILD_TEST=ON -DBUILD_APP=ON -DBUILD_PACKAGE=ON -DCMAKE_TOOLCHAIN_FILE="$CMakeToolchainFile" -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE="$EmscriptenCMakeFile" -DVCPKG_TARGET_TRIPLET="wasm32-emscripten" .
} else {
    cmake -B build -DBUILD_LIB=ON -DBUILD_TEST=ON -DBUILD_APP=ON -DBUILD_PACKAGE=ON -DCMAKE_TOOLCHAIN_FILE="$CMakeToolchainFile" .
}

cmake --build build

if ($RunTests) {
    Set-Location -Path "build"
    ctest --verbose
}

Set-Location -Path $currentDir
