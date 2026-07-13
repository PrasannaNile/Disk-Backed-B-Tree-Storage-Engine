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


# Database Storage Engine Development Log

## Day 1: Physical Storage Layer (Disk Manager)
Designed and implemented the core input/output controller responsible for bridging virtual database operations to the physical hard drive.

### Structural Decisions
*   **Fixed Page Size:** Selected a uniform block size of 4096 bytes (4KB) to match the native hardware sector size of modern storage drives, maximizing file I/O efficiency.
*   **Linear Address Space:** Designed the file system layout as a sequential tape of contiguous pages. This allows instant $O(1)$ coordinate targeting via: 
    $$\text{Byte Offset} = \text{page\_id\_t} \times 4096$$
*   **64-bit Address Precision:** Utilized `uint64_t` / `std::streamoff` calculations to ensure seamless scaling past 2GB files without integer overflow boundaries.
*   **Strict Durability (`.flush()`):** Embedded an explicit flush mechanism into the write pathway to force the operating system's lazy kernel cache to instantly commit dirty blocks directly to disk pladders/flash components, protecting structural index geometry against sudden power faults.

### Component Map
*   `include/storage/disk_manager.hpp` — Contains class definitions and functional signatures.
*   `src/storage/disk_manager.cpp` — File binary stream I/O interface logic.

---

## Day 2: Memory Representation Layer (The Page Wrapper)
Constructed the standardized in-memory envelope used to hold raw data fragments and maintain tracking metadata as pages float between disk and RAM structures.

### Structural Decisions
*   **Architectural Separation:** Insulated the low-level `DiskManager` (which treats files as anonymous byte streams) from the B+ Tree layer (which structures keys and data records). The `Page` class serves as the clean execution frame between them.
*   **Zero-State Safety Guarding:** Implemented low-level `std::memset` block initializations during object constructor allocation and memory recyclability loops to wipe dirty leftover application bits out of raw RAM slots.
*   **State Space Tracking:** Embedded state management variables directly inside the wrapper:
    *   `page_id_`: Maps the memory frame back to its concrete file coordinate location.
    *   `pin_count_`: Counts active thread locks to prevent memory replacement units from dropping frames currently in evaluation.
    *   `is_dirty_`: Flags real-time internal text updates so modifications are stored back to disk prior to frame replacement.

### Component Map
*   `include/storage/page.hpp` — State properties and type constraints (`page_id_t`).
*   `src/storage/page.cpp` — Frame resets and zero-filling implementations.