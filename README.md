# Custom B+ Tree Storage Engine

A modular, low-level database storage engine written in C++ designed to manage tabular index data efficiently using a B+ Tree architecture. This engine bridges the gap between hardware storage layers and analytical/transactional query processors.

---

## 🏗️ Architecture Blueprint

The storage engine is structured using a strict layered layout, separating physical hardware concerns from complex logical structural indices:

```text
       +---------------------------------------------+
       |               B+ Tree Layer                 |  <- (Future Phase)
       |   Organizes primary keys, values, nodes     |
       +---------------------------------------------+
                              |
                              v
       +---------------------------------------------+
       |         Buffer Pool Manager (BPM)           |  <- (Next Phase)
       |   In-Memory Cache, Frame Stealing (LRU)     |
       +---------------------------------------------+
                              |
                              v
       +---------------------------------------------+
       |            Page Representation              |  <- [COMPLETED]
       |  Wraps raw 4KB memory canvases + metadata   |
       +---------------------------------------------+
                              |
                              v
       +---------------------------------------------+
       |               Disk Manager                  |  <- [COMPLETED]
       |  Performs O(1) binary offsets onto drive    |
       +---------------------------------------------+
