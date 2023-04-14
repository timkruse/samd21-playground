# ARM Cortex M0+ - samd21 template (Building with CMake)

**Prerequisites:**

- Atmel/Advanced Software Framework (ASF)
- ARM GCC
- CMake (setup to  be in PATH)
- Ninja-Build (setup to be in PATH)

**Setup:**

Create a ``CMakeUserPresets.json`` file in the root of the project 

```json
{
  "version": 3,
  "configurePresets": [
    {
      "name": "build-ninja-debug",
      "description": "Build with ninja",
      "inherits": "ninja",
      "toolchainFile": "${sourceDir}/toolchain-arm-none-eabi-samd21.cmake",
      "cacheVariables": {
        "ASF_PREFIX": "C:/Users/tkruse/AppData/Local/0_gsi_executables/arm-dev/xdk-asf-3.52.0",
        "CMAKE_C_COMPILER": "C:/Users/tkruse/AppData/Local/0_gsi_executables/arm-dev/arm-gcc-10/bin/arm-none-eabi-gcc.exe",
        "CMAKE_CXX_COMPILER": "C:/Users/tkruse/AppData/Local/0_gsi_executables/arm-dev/arm-gcc-10/bin/arm-none-eabi-g++.exe",
        "CMAKE_EXPORT_COMPILE_COMMANDS": true,
        "CMAKE_BUILD_TYPE": "Debug"
      }
    }
  ]
}
```

and adjust the following variables according to your installation

- `ASF_PREFIX`
- `CMAKE_C_COMPILER`
- `CMAKE_CXX_COMPILER`


**Configure:**

In the root directory of this project run

    cmake --preset build-ninja-debug

**Build**

    cmake --build build/build-ninja-debug 
