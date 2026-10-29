param(
    [string]$SfmlRoot = $env:SFML_ROOT,
    [string]$Compiler = $env:CXX
)

$ErrorActionPreference = "Stop"

if (-not $Compiler) {
    $compilerCommand = Get-Command g++ -ErrorAction SilentlyContinue
    if ($compilerCommand) {
        $Compiler = $compilerCommand.Source
    } elseif (Test-Path "D:\msys2\ucrt64\bin\g++.exe") {
        $Compiler = "D:\msys2\ucrt64\bin\g++.exe"
    }
}

if (-not $SfmlRoot) {
    $knownSfmlPath = "D:\SFML-3.1.0-windows-gcc-14.2.0-mingw-64-bit (1)\SFML-3.1.0"
    if (Test-Path $knownSfmlPath) {
        $SfmlRoot = $knownSfmlPath
    }
}

if (-not $Compiler -or -not (Test-Path $Compiler)) {
    throw "g++.exe was not found. Set the CXX environment variable."
}

if (-not $SfmlRoot -or -not (Test-Path "$SfmlRoot\include\SFML")) {
    throw "SFML was not found. Set the SFML_ROOT environment variable."
}

$buildDirectory = Join-Path $PSScriptRoot "build"
New-Item -ItemType Directory -Force -Path $buildDirectory | Out-Null
$compilerDirectory = Split-Path $Compiler
$env:PATH = "$compilerDirectory;$env:PATH"

$compilerArguments = @(
    "-std=c++17",
    "-Wall",
    "-Wextra",
    "-pedantic",
    "-I$SfmlRoot\include",
    (Join-Path $PSScriptRoot "src\main.cpp"),
    (Join-Path $PSScriptRoot "src\Game.cpp"),
    (Join-Path $PSScriptRoot "src\Hero.cpp"),
    (Join-Path $PSScriptRoot "src\Monster.cpp"),
    "-L$SfmlRoot\lib",
    "-lsfml-graphics",
    "-lsfml-window",
    "-lsfml-system",
    "-o",
    (Join-Path $buildDirectory "PocketHeroes.exe")
)

Write-Host "Compiler: $Compiler"
Write-Host "SFML: $SfmlRoot"
& $Compiler @compilerArguments

if ($LASTEXITCODE -ne 0) {
    throw "Compilation failed with exit code $LASTEXITCODE"
}

@("sfml-graphics-3.dll", "sfml-window-3.dll", "sfml-system-3.dll") | ForEach-Object {
    Copy-Item -Force (Join-Path $SfmlRoot "bin\$_") $buildDirectory
}

@("libgcc_s_seh-1.dll", "libstdc++-6.dll", "libwinpthread-1.dll") | ForEach-Object {
    $runtimeLibrary = Join-Path $compilerDirectory $_
    if (Test-Path $runtimeLibrary) {
        Copy-Item -Force $runtimeLibrary $buildDirectory
    }
}

Write-Host "Build succeeded: $buildDirectory\PocketHeroes.exe" -ForegroundColor Green
