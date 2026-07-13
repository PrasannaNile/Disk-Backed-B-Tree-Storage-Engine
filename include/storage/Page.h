#pragma once

#include <cstdint>
#include <stddef.h>

// Define types for global clarity
using page_id_t = uint32_t;                 // Virtual Page Address Type 

constexpr size_t PAGE_SIZE = 4096;          // 4KB fixed-size pages
constexpr page_id_t INVALID_PAGE_ID = 0;    // empty or null reference flag
