#include "storage/page.h"

#include <cstring>

// Constructor: Initializes the fields and zeroes out the raw memory array
Page::Page() {
    ResetMemory();
}

// ResetMemory: Completely wipes the 4KB buffer and resets all state trackers
void Page::ResetMemory() {
    // Fill all 4096 bytes with zero bits
    std::memset(data_, 0, PAGE_SIZE);

    // Reset the tracking variables back to their factory defaults
    page_id_ = INVALID_PAGE_ID;
    pin_count_ = 0;
    is_dirty_ = false;
}
