#pragma once

#include <list>
#include <unordered_map>
#include <cstddef>
#include <cstdint>


using frame_id_t = uint32_t;


class LRUReplacer {
private:
    // The Chronological Queue: Oldest frame at the front, newest at the back
    std::list<frame_id_t> lru_list_;

    // The Instant Map: Maps a Frame ID to its direct location iterator in the list
    std::unordered_map<frame_id_t, std::list<frame_id_t>::iterator> lru_map_;
public:
    // The replacer needs to know the maximum capacity it should expect
    explicit LRUReplacer(size_t num_pages);
    ~LRUReplacer() = default;
};
