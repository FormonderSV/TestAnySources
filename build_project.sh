#!/bin/bash

# Create a build directory
if [ ! -d "build" ]; then
    mkdir build
fi
cd build

# Give the user a choice of generator
while true; do
    clear
    echo "Select generator:"
    echo ""
    echo "[1] Visual Studio 17 2022"
    echo "[2] Ninja"
    echo "[3] Unix Makefiles"
    echo "[4] Visual Studio Code - CodeBlocks - Ninja"
    echo ""
    read -p "Enter your choice (default is 1): " choice

    # If no input, default to Visual Studio 17 2022
    [ -z "$choice" ] && choice=1

    # Generate project files based on user selection
    case $choice in
        1)
            GENERATOR="Visual Studio 17 2022 -A x64"
            break
            ;;
        2)
            GENERATOR="Ninja"
            break
            ;;
        3)
            GENERATOR="Unix Makefiles"
            break
            ;;
        4)
            GENERATOR="Visual Studio Code - CodeBlocks - Ninja"
            break
            ;;
        *)
            echo "Invalid choice! Please select again."
            read -n 1 -s -r -p "Press any key to continue..."
            ;;
    esac
done

cmake -G "$GENERATOR" ..

# If Ninja or Unix Makefiles are selected, use 'make' to build the project.
if [ "$choice" == "2" ] || [ "$choice" == "3" ]; then
    make
fi

# Let's go back
cd ..

read -n 1 -s -r -p "Press any key to continue..."
