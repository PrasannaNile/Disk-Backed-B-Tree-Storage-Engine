#pragma once

#include "storage/b_plus_tree_page.h"


template <typename KeyType, typename ValueType, typename KeyComparator>
class BPlusTreeInternalPage: public BPlusTreePage {
private:
    using MappingType = std::pair<KeyType, ValueType>;
    // stores the first key, value pair exactly at byte 20
    MappingType array_[1];
public:
    // getter methods
    auto KeyAt(int index) const -> KeyType { return array_[index].first; }
    auto ValueAt(int index) const -> ValueType { return array_[index].second; }

    // setter methods
    void SetKeyAt(int index, const keyType& key) { array_[index].first = key; }
    void SetValueAt(int index, const ValueType& value) { array_[index].second = value; }

    // Lookup Routine
    auto Lookup(const KeyType& key, const KeyComparator &comparator) const -> ValueType {
        // If the page is empty or only contains the dummy index 0
        if(GetSize() <= 1) return array_[0].second;

        // binary search across the valid routing keys
        int low = 1;
        int high  = GetSize() - 1;
        int target_idx = 0;  // Tracks the best matching divider pointer
        
        while(low <= high) {
            int mid = low + ((high - low) >> 1);

            if(comparator(array_[mid].first, key) <= 0) {
                target_idx = mid;
                low = mid + 1;
            } else high = mid - 1;
        }

        return array_[target_idx].second;
    }
};