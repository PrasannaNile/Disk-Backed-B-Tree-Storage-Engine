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


---

# Developer Log: Database Storage Engine Implementation

## Day 3: Mastering the Core Engine Loops (`FetchPage` & `NewPage`)

### Overview
Today was a high-intensity engineering session focused on implementing the two most complex operational loops of the `BufferPoolManager`: `FetchPage(page_id)` and `NewPage(page_id)`. These methods form the core routing layer bridging our volatile physical frames (`RAM`), tracking systems (`page_table_`), cache eviction mechanics (`Replacer`), and persistent storage (`DiskManager`).

### Key Accomplishments

#### 1. Architected the Double-Tiered Frame Hunt
Implemented a robust cache-miss fallback sequence utilized by both fetch and allocation routines to locate empty space in memory safely:
* **Fast-Path Memory Verification:** Checking `free_list_` for unused physical frames instantiated on boot.
* **Slow-Path Page Eviction:** Integrating `replacer.victim()` to identify unpinned frames ready for replacement when RAM is entirely full.

#### 2. Secured Memory Integrity via Forced Flushing
Handled critical multi-state synchronization to avoid data loss during page replacement:
* **Dirty Page Writebacks:** Added checks to evaluate if an evicted page was modified (`IsDirty()`). If true, the system writes the page's modified state back to the disk via `DiskManager` before discarding the frame.
* **State Eviction Tracking:** Successfully purging old metadata mappings from `page_table_` upon victim eviction to eliminate memory leakage.

#### 3. Resolved Disk Manager Allocation Mechanics
Identified and resolved a system abstraction gap where `AllocatePage()` was declared but undefined within the framework:
* Implemented a clean, intuitive internal allocation counter (`num_pages_`) inside `DiskManager`.
* Engineered `DiskManager::AllocatePage()` to safely hand out sequential, unique page identifiers ($0, 1, 2, \dots$) without relying on hidden configurations or risking testing suite crashes.

#### 4. Mastered Dual-Return Output Design
Successfully applied C++ out-pointer referencing (`page_id_t* page_id`) alongside standard return tracking. This elegantly solves the structural limitation of returning dual telemetry to callers: the volatile memory reference (`Page*`) for immediate data manipulation and the persistent integer identifier (`page_id_t`) for structural logging (e.g., child linking inside B+ Trees).

### Technical Decisions & Deep Dives

* **Fixed Array Memory Safety:** Confirmed that running `ResetMemory()` (which invokes a standard memory byte wipe via `memset`) on newly booted frames is fundamentally safe. Because `frames_` is initialized as a fixed-size array on system boot, physical memory allocations for the entire pool exist instantly in RAM, preventing unexpected segmentation faults.
* **Scoping the Eviction Block:** Fixed a structural bug where data eviction logic was running universally outside the fallback paths. Relocating data cleanup routines strictly inside the `else { ... replacer.victim() ... }` block ensures innocent, fresh frames pulled from the free list are never subjected to invalid memory purges.

### Status Check
* **FetchPage Implementation:** Complete & verified.
* **NewPage Implementation:** Complete & verified.
* **Disk Allocation Engine:** Implemented & linked.
* **Next Target:** Deletion mechanisms, clean flushing utilities, and local multi-threaded lock integrations.