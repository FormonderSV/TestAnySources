cmake_minimum_required(VERSION 3.13)

# Project Name
set(This TestAnySources)
project(${This} C CXX)

# Add Include Directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

# GoogleTest parameters
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(BUILD_GTEST ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(gtest_disable_pthreads ON)
set(GOOGLETEST_VERSION 1.10.0)

# Adding Google Test as a subdirectory
add_subdirectory(external/googletest/googletest)

# Source files
file(GLOB_RECURSE SOURCES "src/modules/*.cpp")

# Test files (excluding main test runner)
file(GLOB_RECURSE TEST_SOURCES "tests/units/*.cpp" "src/modules/*.cpp")

# Main Executable
add_executable(${This} ${SOURCES} src/main.cpp)
# Link necessary libraries (like GoogleTest) to main project if necessary

# Test Executable
add_executable(${This}Tests ${TEST_SOURCES} tests/main.cpp)
target_link_libraries(${This}Tests gtest_main)

# This will make the tests available to CTest and ctest command
include(GoogleTest)
gtest_discover_tests(${This}Tests)
