@echo off

REM Delete the build directory
if exist build (
    rmdir /s /q build
    echo "Folder 'build' deleted!"
) else (
    echo "Folder 'build' not found."
)

REM Delete other temporary or generated files if needed
if exist CMakeCache.txt (
    del /f /q CMakeCache.txt
    echo "File 'CMakeCache.txt' deleted!"
)

if exist CMakeFiles (
    rmdir /s /q CMakeFiles
    echo "Folder 'CMakeFiles' deleted!"
)

pause
