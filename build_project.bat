@echo off
setlocal enabledelayedexpansion

REM Create a build directory
if not exist build (
    mkdir build
)
cd build

REM Give the user a choice of generator
:choice
cls
echo Select generator:
echo.
echo [1] Visual Studio 17 2022
echo [2] Ninja
echo [3] Unix Makefiles
echo [4] Visual Studio Code - CodeBlocks - Ninja
echo.
set /p choice="Enter your choice (default is 1): "

REM If no input, default to Visual Studio 17 2022
if "!userChoice!"=="" set userChoice=1

REM Generate project files based on user selection
set GENERATOR_NAME=Visual Studio 17 2022

if "%userChoice%"=="1" (
    set GENERATOR="Visual Studio 17 2022" -A x64
) else if "!choice!"=="2" (
    set GENERATOR_NAME=Ninja
) else if "!choice!"=="3" (
    set GENERATOR_NAME=Unix Makefiles
) else if "!choice!"=="4" (
    set GENERATOR_NAME="Visual Studio Code - CodeBlocks - Ninja"
) else (
    echo Invalid choice! Please select again.
    pause
    goto choice
)

cmake -G %GENERATOR% ..

REM Extracting the project name from the generated files
for %%i in (*.sln) do set SLN_NAME=%%i

if not "%SLN_NAME%"=="" (
    echo Solution Name: !SLN_NAME!
    REM Building the Solution with MSBuild
    msbuild !SLN_NAME! /p:Configuration=Release
)

REM Let's go back
cd ..

pause
