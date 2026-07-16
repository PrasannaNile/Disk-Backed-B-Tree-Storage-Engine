#pragma once

template <typename KeyType>
struct IntComparator {
    inline int operator()(const KeyType& lhs, const keyType& rhs) {
        if(lhs < rhs) return -1;
        if(lhs > rhs) return 1;
        return 0;
    }
};