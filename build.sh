#!/bin/bash

echo
# Prompt for build type
read -p "Choose build type (1 for Release, 2 for Debug): " build_choice
if [ "$build_choice" -eq 1 ]; then
    BUILD_TYPE="Release"
elif [ "$build_choice" -eq 2 ]; then
    BUILD_TYPE="Debug"
else
    echo "Invalid build type choice. Please enter 1 or 2."
    exit 1
fi

echo
echo "Selected build type: $BUILD_TYPE"
echo

# Prompt for CMake generator
read -p "Choose CMake Generator (1 for Ninja, 2 for Make): " generator_choice
if [ "$generator_choice" -eq 1 ]; then
    CMAKE_GENERATOR="Ninja"
elif [ "$generator_choice" -eq 2 ]; then
    CMAKE_GENERATOR="Unix Makefiles"
else
    echo "Invalid CMake Generator choice. Please enter 1 or 2."
    exit 1
fi

echo
echo "Selected CMake Generator: $CMAKE_GENERATOR"
echo

NUM_THREADS=$(($(nproc) - 2))

# Run CMake
cmake -G "$CMAKE_GENERATOR" \
    -D CMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -D CMAKE_INSTALL_PREFIX="./__build_out__/$BUILD_TYPE" \
    -B "./__build_dir__/$BUILD_TYPE" .

cmake --build "__build_dir__/$BUILD_TYPE" --config "$BUILD_TYPE" -j"$NUM_THREADS"
cmake --build "__build_dir__/$BUILD_TYPE" -- install

# end of file
