# X-Plane-11-Window-API-Sample

[![CMake](https://github.com/sparker256/X-Plane-11-Window-API-Sample/actions/workflows/cmake.yml/badge.svg)](https://github.com/sparker256/X-Plane-11-Window-API-Sample/actions/workflows/cmake.yml)

Using X-Plane 11 Window API Sample from

https://developer.x-plane.com/code-sample/x-plane-11-window-api-sample/

Create a CMake project with GitHub Actions

## Building X-Plane-11-Window-API-Sample from source

### On windows

1.  Install required software using [Chocolatey](https://chocolatey.org/) using admin command prompt:

    ```
    choco install git cmake
    choco install mingw --version 8.1.0
    ```

    You can also install the same programs manually if you prefer.

2.  Checkout and configure the project:

    ```
    git clone https://github.com/sparker256/X-Plane-11-Window-API-Sample.git
    cd X-Plane-11-Window-API-Sample
    cmake -G "MinGW Makefiles" -S .\src -B .\build -DCMAKE_BUILD_TYPE=RelWithDebInfo
    ```

3.  Build the project and copy the plugin DLL into the appropriate directory:

    ```
    cmake --build .\build
    cp .\build\win.xpl .\XP11WindowApiSample\64\
    ```

### On Ubuntu:

1. Install required software:

   ```
   sudo apt-get install -y --no-install-recommends build-essential cmake git freeglut3-dev libudev-dev libopenal-dev

   ```

2. Checkout and configure the project:

   ```
   git clone https://github.com/sparker256/X-Plane-11-Window-API-Sample.git
   cd X-Plane-11-Window-API-Sample
   cmake -S ./src -B ./build -DCMAKE_BUILD_TYPE=RelWithDebInfo
   ```

3. Build the project and copy the plugin DLL into the appropriate directory:

   ```
   cmake --build ./build
   cp ./build/lin.xpl ./XP11WindowApiSample/64/
   ```

### On OS X:

1. Install XCode, Git, CMake (Homebrew can be convenient for this).

2. Checkout and configure the project:

   ```
   git clone https://github.com/sparker256/X-Plane-11-Window-API-Sample.git
   cd X-Plane-11-Window-API-Sample
   cmake -S ./src -B ./build -DCMAKE_BUILD_TYPE=RelWithDebInfo
   ```

3. Build the project and copy the plugin DLL into the appropriate directory:

   ```
   cmake --build ./build
   cp ./build/mac.xpl ./XP11WindowApiSample/64/
   ```
