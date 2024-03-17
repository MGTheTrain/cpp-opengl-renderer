param(
    [string]$CMakeToolchainFile,
    [switch]$RunTests,
    [switch]$Help,
    [switch]$WebBuild
)

function Show-Help {
    echo "Usage: .\compile_source_code.ps1 -CMakeToolchainFile <Path> [-RunTests] [-WebBuild] [-Help]"
    echo "  -CMakeToolchainFile <Path>   Path to CMAKE_TOOLCHAIN_FILE"
    echo "  -RunTests                    Run tests after building (optional)"
    echo "  -WebBuild                    Build for web (with emscripten) instead of Desktop (optional)"
    echo "  -Help                        Display this help message"
}

if ($Help) {
    Show-Help
    exit
}

$currentDir = Get-Location
Set-Location -Path "$PSScriptRoot/../../.."

if ($WebBuild) {
    echo ""'$env:PATH += <Path to emsdk folder>/emsdk_env.bat'""
    mkdir -f build
    Set-Location -Path 
    emcmake -DBUILD_LIB=ON -DBUILD_TEST=ON -DBUILD_APP=ON -DBUILD_PACKAGE=ON -DBUILD_WEB=ON .. # ninja or mingw32-make generator required on Windows OS
    emcmake --build build
} else {
    if (-not $CMakeToolchainFile) {
        $CMakeToolchainFile = Read-Host "Enter the path to CMakeToolchainFile, e.g. 'D:\c++ repos\dependencies\vcpkg\scripts\buildsystems\vcpkg.cmake'"
    }
    cmake -B build -DBUILD_LIB=ON -DBUILD_TEST=ON -DBUILD_APP=ON -DBUILD_PACKAGE=ON -DCMAKE_TOOLCHAIN_FILE="$CMakeToolchainFile" .
    cmake --build build
    if ($RunTests) {
        Set-Location -Path "build"
        ctest --verbose
    }
}

Set-Location -Path $currentDir
