#!/bin/bash

# Delete the build directory
if [ -d "build" ]; then
    rm -rf build
    echo "Folder 'build' deleted!"
else
    echo "Folder 'build' not found."
fi

# Delete other temporary or generated files if needed
if [ -f "CMakeCache.txt" ]; then
    rm -f CMakeCache.txt
    echo "File 'CMakeCache.txt' deleted!"
fi

if [ -d "CMakeFiles" ]; then
    rm -rf CMakeFiles
    echo "Folder 'CMakeFiles' deleted!"
fi

read -p "Press [Enter] to continue..."
