# fmc 

Flash Mananeger / Cache

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## Introduction

A minimal flash storage interface library and cache layer to manage flash storage with differential page and block sizes.

For example, on the Raspberry Pi Pico, the onboard flash storage is 2Mb in size, arranged in blocks of 4096 bytes. Each of those
blocks can be seen as 16 pages of 256 bytes each, which is more useful for a minimal file system.

## Building

flash-manager-cache uses [CMake](https://cmake.org/) so building is trivial:

```bash
mkdir build
cd build
cmake ..
make
```

You'll find the `libfcm.a` in the `build/lib` folder. 

## Testing

flash-manager-cache uses [GoogleTest](https://github.com/google/googletest) which is installed by **CMake**:

```bash
mkdir build
cd build
cmake ..
make
./fcm_test
```