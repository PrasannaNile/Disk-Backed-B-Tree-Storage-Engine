#pragma once

#include "storage/page.h"

#include <cstdint>

enum class BPlusTreePageType : uint32_t {INTERNAL = 0, LEAF = 1};


class BPlusTreePage {
private:
    // Tracks if this page is a Leaf or Internal node.
    BPlusTreePageType page_type_;
    // The current number of active keys inside the node.
    uint32_t size_;
    // The max number of keys allowed before a split occurs.
    uint32_t max_size_;
    // Page ID of the parent node
    page_id_t parent_page_id_;
    // Self-contained Page ID
    page_id_t page_id_;
public:
    // Helper methods to inspect and modify the header fields
    auto IsLeafPage() const -> bool { return page_type_ == BPlusTreePageType::LEAF; }
    auto GetPageType() const -> BPlusTreePageType { return page_type_; }
    void SetPageType(BPlusTreePageType type) { page_type_ = type; }

    auto GetSize() const -> uint32_t { return size_; }
    void SetSize(uint32_t size) { size_ = size; }
    void IncreaseSize(uint32_t amount) { size_ += amount; }

    auto GetMaxSize() const -> uint32_t { return max_size_; }
    void SetMaxSize(uint32_t max_size) { max_size_ = max_size; }

    auto GetParentPageId() const -> page_id_t { return parent_page_id_; }
    void SetParentPageId(page_id_t parent_page_id) { parent_page_id_ = parent_page_id; }

    auto GetPageId() const -> page_id_t { return page_id_; }
    void SetPageId(page_id_t page_id) { page_id_ = page_id; }

};