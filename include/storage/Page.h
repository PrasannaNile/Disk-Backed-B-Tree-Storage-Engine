#pragma once

#include <cstdint>
#include <stddef.h>

// Define types for global clarity
using page_id_t = uint32_t;                 // Virtual Page Address Type 

constexpr size_t PAGE_SIZE = 4096;          // 4KB fixed-size pages
constexpr page_id_t INVALID_PAGE_ID = 0;    // empty or null reference flag


class Page {
private:
    char data_[PAGE_SIZE];
    page_id_t page_id_ { INVALID_PAGE_ID };
    int pin_count_ { 0 };
    bool is_dirty_ { false };
    
public:
    // 1. Constructor: Initializes a blank, unused memory frame
    Page();

    // 2. Destructor: Default clean up
    ~Page() = default;

    // 3. Clear/Reset: Wipes the frame so it can be safely reused for a different file page
    void ResetMemory();

    // 4. Getters: Accessors for the internal status fields
    page_id_t GetPageId() const { return page_id_; }
    int GetPinCount() const { return pin_count_; }
    bool IsDirty() const { return is_dirty_; }
    const char *GetData() const { return data_; }
    char *GetData() { return data_; }

    // 5. Setters & Modifiers: Updaters for the internal status fields
    void SetPageId(page_id_t page_id) { page_id_ = page_id; }
    void SetDirty(bool is_dirty) { is_dirty_ = is_dirty; }
    void IncreasePinCount() { pin_count_++; }
    void DecreasePinCount() { if (pin_count_ > 0) pin_count_--; }

};