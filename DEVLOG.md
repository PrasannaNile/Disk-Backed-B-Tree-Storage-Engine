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


---

# Developer Log: Day 5 - B+ Tree Node Layout Design

## Overview
Today's focus was entirely conceptual, shifting gears from the low-level physical storage layers (Disk and Buffer Pool Managers) to the foundational architecture of the B+ Tree itself. No code was written to maintain a sustainable development pace.

## Key Accomplishments
*   **DiskManager Refactoring:** Successfully diagnosed and patched critical initialization and edge-case error bugs within the physical page file manager constructor.
*   **Node Architecture Defined:** Conceptualized the 4KB page memory layout splits required for B+ Tree traversal.
*   **Structural Differentiation Matrix:** 
    *   **Leaf Nodes:** Defined to map distinct user keys directly to data record identifiers, alongside a trailing `next_page_id` pointer for range queries.
    *   **Internal Nodes:** Defined strictly as routing nodes that store key-to-page-ID mappings to guide vertical tree traversal down to the leaves.

## Next Steps
*   Create header files defining the base `BPlusTreePage` layout class.
*   Implement explicit binary serialization layouts for individual internal and leaf structs.

---


# Developer Log: Day 6 - Shared Node Header Design

## Overview
Continued the conceptual exploration of the B+ Tree architecture, focusing on the shared structural layout of database pages. Clarified the physical mental model of node traversal and memory partitioning to prevent future implementation mistakes.

## Key Accomplishments
*   **Tree Simulation Clarification:** Resolved confusion regarding how randomly scattered physical disk pages logically simulate a sorted tree structure using internal signpost keys and Page ID pointers.
*   **Base Class Architecture (`BPlusTreePage`):** Defined the role of a unified base class to enforce memory uniformity, enabling predictable byte offsets and polymorphism across node variants.
*   **Header Metadata Layout:** Specified the exact fixed-size fields required at byte offset zero for every 4KB page:
    *   `page_type` (Leaf vs. Internal)
    *   `size` (Current key count)
    *   `max_size` (Split threshold)
    *   `parent_page_id` (Upward traversal pointer)
    *   `page_id` (Self-verification pointer)
*   **Memory Alignment:** Modeled how zero-cost pointer casting will map these structured fields directly over raw binary page buffers without runtime overhead.

## Next Steps
*   Define the structural differences in the payload section between Internal and Leaf nodes.
*   Begin mapping out the C++ structure definitions for the shared header.

---


# Developer Log: Day 7 - Implementation of Base Node Layout

## Overview
Shifted from pure concepts to concrete structural engineering by defining and implementing the base class structure that governs memory layout for all B+ Tree nodes.

## Key Accomplishments
*   **Enumerated Node Typings:** Declared the `BPlusTreePageType` enum class using an explicit 4-byte (`uint32_t`) backing type to strictly bound header memory.
*   **Base Layout Implementation (`BPlusTreePage`):** Codified the primary header struct to measure exactly 20 bytes with total physical alignment stability:
    *   `page_type_` (0-3 bytes)
    *   `size_` (4-7 bytes)
    *   `max_size_` (8-11 bytes)
    *   `parent_page_id_` (12-15 bytes)
    *   `page_id_` (16-19 bytes)
*   **Modern C++ Idioms Applied:** Utilized C++11 trailing return types (`auto Func() -> Type`) for strict codebase cleanliness and applied `const` qualifiers to guarantee read-only pointer safety on accessors.

## Next Steps
*   Define the structural memory layouts for `BPlusTreeInternalPage`.
*   Establish data-mapping payloads for `BPlusTreeLeafPage`.

---


# Devlog: Day 8 - B+ Tree Internal Page Structural Layout & Lookup

## Objective
Implement the memory layout, tracking mechanisms, and search routing routine (`Lookup`) for the `BPlusTreeInternalPage`. This class overlays directly onto raw 4096-byte memory pages managed by the `BufferPoolManager`.

---

## Architectural Insights & Engineering Decisions

### 1. In-Memory Direct Mapping (`reinterpret_cast`)
Instead of using standard heap allocations (`new`), our internal nodes are cast directly over raw 4KB memory buffers using `reinterpret_cast`. 
* **The Constraint:** C++ constructors do not fire during a `reinterpret_cast`. 
* **The Solution:** Implemented an explicit `Init()` method to safely wipe garbage data and structure the 20-byte base page header metadata whenever a page frame is freshly allocated or repurposed.

### 2. The Index 0 Value Realignment (The 0th Pointer Problem)
Internal nodes function as routing mechanisms where $N$ keys (dividers) logically map to $N + 1$ child pointers (lanes). 
* Because our C++ storage representation mandates rigid `std::pair<KeyType, ValueType>` elements, we face an asymmetric pairing dilemma.
* **The Design choice:** Index 0 is designated as a dummy slot. Its `Key` is ignored, and its `Value` (child page ID) is utilized to track "Lane 0"—the pointer handling all elements strictly less than the first real routing key at Index 1.

### 3. Logarithmic Key Routing via Binary Search
Linear scans ($O(N)$) degrade performance linearly as the page size approaches maximum capacity. 
* Implemented `Lookup()` using binary search to scale efficiency to $O(\log N)$.
* Implemented a fallback tracking state (`target_idx = 0`). If a target search key is smaller than the first real divider at Index 1, the search correctly drops through to the 0th ignored pointer.

### 4. Comparison Abstraction (`KeyComparator`)
The page architecture must remain entirely content-blind to support varied primary key types (integers, strings, composites). The internal page delegates comparison evaluations to an isolated `KeyComparator` function object passed down from the top-level index execution layer.

---

## Implementation Details

### Component Layout
* **Base Class:** `BPlusTreePage` (Manages shared 20-byte header metrics: Page Type, Size, Max Size, Parent Page ID, Page ID).
* **Derived Class:** `BPlusTreeInternalPage` (Manages routing array buffers and traversal logic).

### Core Methods Added
* `Init(...)`: Reinitializes the page header type to `INTERNAL_PAGE` and zeros out tracking sizes.
* `Lookup(const KeyType &key, const KeyComparator &comparator)`: Executes a binary search across valid boundary dividers to determine the matching `page_id_t` for structural downward traversal.

---

## Verification & Status
* **State:** Core internal page mechanics are verified and sealed. 
* **Next Step:** Day 9 will focus on building the `BPlusTreeLeafPage` structure, transitioning our layout logic from handling routing pointer values (`page_id_t`) to handling actual record/tuple storage mappings.