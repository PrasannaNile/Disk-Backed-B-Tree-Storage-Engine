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

    size_t capacity_{};
public:
    // The replacer needs to know the maximum capacity it should expect
    explicit LRUReplacer(size_t num_pages);
    ~LRUReplacer() = default;

    // add new node to the end of the replacer list 
    void unpin(frame_id_t frame_id);

    // replace page from the replacer list, a very first node (least recently used)
    bool victim(frame_id_t* frame_id);

    // page is already in RAM, but it was unpinned and again query refers it
    void pin(frame_id_t frame_id); 

    // how many pages are ready evict
    size_t pages_ready_to_evict() const;
};
