param(
    [string]$CMakeToolchainFile,
    [string]$RevisionNumber = "",
    [switch]$RunTests,
    [switch]$Release,
    [switch]$Help
)

if ($Help) {
    Write-Host "Usage: .\build.ps1 -CMakeToolchainFile <Path> [-RunTests] [-Release] [-RevisionNumber <rev>] [-Help]"
    exit 0
}

$BuildType = if ($Release) { "Release" } else { "Debug" }

Set-Location -Path "$PSScriptRoot/.."

if (-not $CMakeToolchainFile) {
    $CMakeToolchainFile = Read-Host "Enter the path to CMakeToolchainFile"
}

$revisionArg = if ($RevisionNumber) { @("-DREVISION_NUMBER=$RevisionNumber") } else { @() }

if ($RunTests) {
    Write-Host "INFO: Configuring combined test + app build (Debug)"
    cmake -B build `
        -DBUILD_LIB=ON -DBUILD_TEST=ON -DBUILD_APP=ON -DBUILD_PACKAGE=ON `
        -DCMAKE_BUILD_TYPE="$BuildType" `
        -DCMAKE_TOOLCHAIN_FILE="$CMakeToolchainFile" `
        @revisionArg `
        .
    cmake --build build --parallel --config "$BuildType"
    Write-Host "INFO: Running tests"
    Set-Location -Path build
    ctest -C "$BuildType" --verbose
    Set-Location -Path "$PSScriptRoot/.."
} else {
    Write-Host "INFO: Configuring app-only build ($BuildType)"
    cmake -B build `
        -DBUILD_LIB=ON -DBUILD_TEST=OFF -DBUILD_APP=ON -DBUILD_PACKAGE=ON `
        -DCMAKE_BUILD_TYPE="$BuildType" `
        -DCMAKE_TOOLCHAIN_FILE="$CMakeToolchainFile" `
        @revisionArg `
        .
    cmake --build build --parallel --config "$BuildType"
}

Write-Host "INFO: Build ($BuildType) complete"
