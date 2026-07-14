#pragma once

#include "storage/page.h"

#include <cstddef>
#include <cstdint>
#include <vector>
#include <unordered_map>


// Type Aliases and Constants
using frame_id_t = uint32_t;
constexpr frame_id_t INVALID_FRAME_ID = -1;
constexpr size_t POOL_SIZE = 10; // Fixed capacity layout


class BufferPoolManager {
public:
    // Basic Constructor & Destructor shell
    BufferPoolManager() = default;
    ~BufferPoolManager() = default;

private:
    // Step 3.1.4: The physical array of RAM slots
    Page frames_[POOL_SIZE];

    // The Free List tracking empty slots
    std::vector<frame_id_t> free_list_;

    // Maps Page IDs on disk to their current Frame ID slot in RAM
    std::unordered_map<page_id_t, frame_id_t> page_table_t;
};


