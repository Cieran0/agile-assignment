#!/bin/bash

# Get raylib paths
RAYLIB_PATH="/opt/homebrew/Cellar/raylib"
RAYLIB_VERSION=$(ls ${RAYLIB_PATH} | sort -V | tail -n 1)
RAYLIB_INCLUDE="${RAYLIB_PATH}/${RAYLIB_VERSION}/include"
RAYLIB_LIB="${RAYLIB_PATH}/${RAYLIB_VERSION}/lib"

# Get OpenSSL paths for Apple Silicon (M1/M2) Macs
OPENSSL_PATH="/opt/homebrew/opt/openssl@3"
if [ ! -d "$OPENSSL_PATH" ]; then
    # Fallback to Intel Mac path
    OPENSSL_PATH="/usr/local/opt/openssl@3"
fi

# Print paths for debugging
echo "Using raylib version: ${RAYLIB_VERSION}"
echo "Include path: ${RAYLIB_INCLUDE}"
echo "Library path: ${RAYLIB_LIB}"
echo "OpenSSL path: ${OPENSSL_PATH}"

# Compile with all necessary flags
g++ -std=c++17 atmUi.cpp atmUtil.cpp -o atm \
    -I${RAYLIB_INCLUDE} \
    -L${RAYLIB_LIB} \
    -I${OPENSSL_PATH}/include \
    -L${OPENSSL_PATH}/lib \
    -Wno-deprecated-declarations \
    -Wno-gnu-designator \
    -lssl -lcrypto \
    -framework OpenGL \
    -framework Cocoa \
    -framework IOKit \
    -framework CoreVideo \
    -lraylib

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful!"
    # Make the output file executable
    chmod +x atm
else
    echo "Compilation failed!"
    exit 1
fi
