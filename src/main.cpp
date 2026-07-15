#include "storage/buffer_pool_manager.h"
#include "storage/disk_manager.h" 


#include <iostream>
#include <cassert>

int main() {
    std::cout << "=== Running Buffer Pool Manager Sanity Tests ===" << std::endl;

    // 1. Initialize the system (Assuming DiskManager constructor takes a dummy file name or nothing)
    // If your DiskManager constructor needs arguments, adjust it here.
    DiskManager disk_manager("test_db.bin"); 
    LRUReplacer replacer(10);
    BufferPoolManager bpm(&disk_manager, &replacer); 
    // If your constructor assigns the disk manager, make sure it links up.
    // e.g., BufferPoolManager bpm(&disk_manager); if it takes a pointer.

    page_id_t page_id_temp;

    // Test Case 1: Create a brand new page
    Page* page0 = bpm.NewPage(&page_id_temp);
    assert(page0 != nullptr);
    assert(page_id_temp == 0);
    std::cout << "[PASS] Successfully allocated Page ID: " << page_id_temp << std::endl;

    // Write some dummy data to the page
    snprintf(page0->GetData(), 100, "Hello from Day 4 Storage Engine!");
    
    // Test Case 2: Unpin the page and mark it dirty
    bool unpin_success = bpm.UnpinPage(0, true);
    assert(unpin_success == true);
    std::cout << "[PASS] Successfully unpinned Page 0 and set dirty bit." << std::endl;

    // Test Case 3: Flush the page to save it
    bool flush_success = bpm.flushPage(0);
    assert(flush_success == true);
    std::cout << "[PASS] Successfully flushed Page 0 to disk." << std::endl;

    // Test Case 4: Fetch the page back and check if data matches
    Page* page0_reloaded = bpm.FetchPage(0);
    assert(page0_reloaded != nullptr);
    std::string data(page0_reloaded->GetData());
    assert(data == "Hello from Day 4 Storage Engine!");
    std::cout << "[PASS] Verified data integrity: \"" << data << "\"" << std::endl;

    // Clean up pin count
    bpm.UnpinPage(0, false);

    std::cout << "\n🎉 ALL DAY 4 SANITY TESTS PASSED SUCCESSFULLY! 🎉" << std::endl;
    return 0;
}