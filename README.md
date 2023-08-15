# TestAnySources

Welcome to the `TestAnySources`. This document will guide you through the setup, project file generation, and intermediate files cleaning.

## Table of Contents

- [Prerequisites](#prerequisites)
- [Setting Up the Project](#setting-up-the-project)
- [Generating Project Files](#generating-project-files)
- [Cleaning Intermediate Files](#cleaning-intermediate-files)

## Prerequisites

Before you begin, ensure you have met the following requirements:

- **CMake** and **msbuild** should be installed on your machine.
    - For **Windows**: 
      - Ensure both `cmake` and `msbuild` are accessible from the command line. This usually means adding their installation directories to the `PATH` environment variable. 
        - If you have installed Visual Studio, `msbuild` should already be available from the command line. 
        - For `cmake`, you might need to add it manually.
      - If you're unfamiliar with adding paths to the `PATH` environment variable in Windows, you can follow [this guide](https://www.architectryan.com/2018/03/17/add-to-the-path-on-windows-10/).
    - For **Unix/Linux/macOS**: `cmake` should be available from the package manager, and there is no need for `msbuild` unless you are targeting a .NET build (which is not common on these platforms for C++ development).


- Install [CMake](https://cmake.org/download/)
- Ensure you have the required build tools installed based on your preferred generator (e.g., Visual Studio, Ninja, etc.)
- [Optional] For Unix/Linux/macOS users: Make sure you have `bash` installed.

## Setting Up the Project

1. Clone this repository to your local machine.
    ```bash
    git clone https://github.com/FormonderSV/TestAnySources.git
    cd TestAnySources
    ```

2. Update and initialize the submodules:
    - For Windows:
        ```bash
        update_submodules.bat
        ```
    - For Unix/Linux/macOS:
        ```bash
        ./update_submodules.sh
        ```

3. Ensure that the scripts are executable:
    - For Windows: Scripts have `.bat` extension.
    - For Unix/Linux/macOS: Make the `.sh` scripts executable (if they're not already):
        ```bash
        chmod +x build_project.sh clear_intermediate_files.sh update_submodules.sh
        ```
        
## Generating Project Files

### Windows

Run the following script from the root directory of the project:
    ```
    build_project.bat
    ```

Follow the prompts to choose your preferred generator. If you don't make a selection, it will default to "Visual Studio 17 2022".

### Unix/Linux/macOS

Execute the bash script:
    ```
    ./build_project.sh
    ```

As with the Windows script, follow the prompts to select your generator. 

## Cleaning Intermediate Files

It's a good practice to occasionally clean up intermediate files generated during the build process, especially when switching between different build configurations or systems.

### Windows

To clean up intermediate files:
    ```
    clear_intermediate_files.bat
    ```

### Unix/Linux/macOS

On Unix-based systems, run:
    ```
    ./clear_intermediate_files.sh
    ```

This will remove build directories and other temporary files.

## Conclusion

Now, you should have a proper understanding of how to set up, generate project files, and maintain a clean environment for the `TestAnySources`. For further details or issues, please refer to the project's documentation or raise an issue on the GitHub repository.
