param(
    [string]$CMakeToolchainFile,
    [string]$RevisionNumber = "",
    [switch]$RunTests,
    [switch]$Release,
    [switch]$Help
)

function Show-Help {
    Write-Host "Usage: .\build.ps1 -CMakeToolchainFile <Path> [-RunTests] [-Release] [-RevisionNumber <rev>] [-Help]"
    Write-Host "  -CMakeToolchainFile <Path>   Path to CMAKE_TOOLCHAIN_FILE"
    Write-Host "  -RunTests                    Run tests after building (always uses Debug)"
    Write-Host "  -Release                     Build in Release mode (default: Debug)"
    Write-Host "  -RevisionNumber <rev>        Revision suffix appended to package version (e.g. alpha-42)"
    Write-Host "  -Help                        Display this help message"
}

if ($Help) {
    Show-Help
    exit 0
}

$BuildType = if ($Release) { "Release" } else { "Debug" }

$currentDir = Get-Location
Set-Location -Path "$PSScriptRoot/.."

if (-not $CMakeToolchainFile) {
    $CMakeToolchainFile = Read-Host "Enter the path to CMakeToolchainFile, e.g. 'D:\repos\vcpkg\scripts\buildsystems\vcpkg.cmake'"
}

$revisionArg = if ($RevisionNumber) { "-DREVISION_NUMBER=$RevisionNumber" } else { "" }

# Tests always build in Debug regardless of -Release flag
if ($RunTests) {
    Write-Host "INFO: Configuring test build (Debug)"
    cmake -B build-test `
        -DBUILD_LIB=ON -DBUILD_TEST=ON -DBUILD_APP=OFF -DBUILD_PACKAGE=OFF `
        -DCMAKE_BUILD_TYPE=Debug `
        -DCMAKE_TOOLCHAIN_FILE="$CMakeToolchainFile" `
        .
    cmake --build build-test --parallel --config Debug
    Write-Host "INFO: Running tests"
    Set-Location -Path build-test
    ctest -C Debug --verbose
    Set-Location -Path "$PSScriptRoot/.."
}

Write-Host "INFO: Configuring app build ($BuildType)"
$cmakeArgs = @(
    "-B", "build",
    "-DBUILD_LIB=ON", "-DBUILD_TEST=OFF", "-DBUILD_APP=ON", "-DBUILD_PACKAGE=ON",
    "-DCMAKE_BUILD_TYPE=$BuildType",
    "-DCMAKE_TOOLCHAIN_FILE=$CMakeToolchainFile"
)
if ($RevisionNumber) {
    $cmakeArgs += "-DREVISION_NUMBER=$RevisionNumber"
}
cmake @cmakeArgs .
cmake --build build --parallel --config "$BuildType"
Write-Host "INFO: Build ($BuildType) complete"

Set-Location -Path $currentDir
