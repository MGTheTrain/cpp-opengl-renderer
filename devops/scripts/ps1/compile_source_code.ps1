param(
    [string]$CMakeToolchainFile,
    [switch]$RunTests,
    [switch]$Help
)

function Show-Help {
    echo "Usage: .\compile_source_code.ps1 -CMakeToolchainFile <Path> [-RunTests] [-Help]"
    echo "  -CMakeToolchainFile <Path>   Path to CMAKE_TOOLCHAIN_FILE"
    echo "  -RunTests                     Run tests after building (optional)"
    echo "  -Help                         Display this help message"
}

if ($Help) {
    Show-Help
    exit
}

if ($CMakeToolchainFile -eq $null) {
    $CMakeToolchainFile = Read-Host "Enter the path to CMAKE_TOOLCHAIN_FILE" 
}

$currentDir = Get-Location
Set-Location -Path "$PSScriptRoot/../../.."
cmake -B build -DBUILD_LIB=ON -DBUILD_TEST=ON -DBUILD_APP=ON -DBUILD_PACKAGE=ON -DCMAKE_TOOLCHAIN_FILE="$CMakeToolchainFile" .

cmake --build build

if ($RunTests) {
    Set-Location -Path "build"
    ctest
}

Set-Location -Path $currentDir