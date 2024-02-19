param(
    [string]$CMakeToolchainFile
)

if ($CMakeToolchainFile -eq $null) {
    $CMakeToolchainFile = Read-Host "Enter the path to CMAKE_TOOLCHAIN_FILE" 
}
$currentDir = Get-Location
Set-Location -Path "$PSScriptRoot/../../.."
cmake -B build -DBUILD_LIB=ON -DBUILD_TEST=ON -DBUILD_APP=ON -DCMAKE_TOOLCHAIN_FILE="$CMakeToolchainFile" .
cmake --build build
Set-Location -Path $currentDir
echo "Example: .\compile_source_code.ps1 -CMakeToolchainFile `"D:\c++ repos\dependencies\vcpkg\scripts\buildsystems\vcpkg.cmake`""