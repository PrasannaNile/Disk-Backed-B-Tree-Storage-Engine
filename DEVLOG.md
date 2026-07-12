# Developer Log: Day 0.

## 1. Toolchain & Environment Fixes
* **Issue:** CMake defaulted to Visual Studio `NMake` toolchain, failing to find the MSYS64 environment.
* **Resolution:** Wiped `/build` sandbox and forced GCC/G++ explicitly via PowerShell:
  ```powershell
  Remove-Item -Recurse -Force *
  cmake -G "MinGW Makefiles" ..
  cmake --build .

## 2. CMake Configuration
* **C++ 17:** Forces to used `c++ 17` version for smart pointers and enum classes
* **Binary Files:** Routes all .obj and .exe files to `/build` and `/bin` folder
* **Entry point:** main.cpp is entry point and add to add_executable with all 
.cpp files to generate combined .exe file