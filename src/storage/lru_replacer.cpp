#include "storage/lru_replacer.h"

#include <cassert>

// Constructor: reserves slots so map does not have to rehash (same time)
LRUReplacer::LRUReplacer(size_t no_of_pages) : capacity_{no_of_pages} {
    lru_map_.reserve(no_of_pages);
}


// unpin(): replacer tracks this frame for replacement policy
void LRUReplacer::unpin(frame_id_t frame_id) {
    assert(frame_id >= 0 && frame_id < this->capacity_ && "Frame Id in out of range");

    auto it = lru_map_.find(frame_id);

    // frame is not yet unpinned
    if(it == lru_map_.end()) {
        // unpinned it at the end (most recently used)
        lru_list_.insert(lru_list_.end(), frame_id);
        lru_map_[frame_id] = std::prev(lru_list_.end());

    } else {
        // frame already been pin, remove and append at the end 
        lru_list_.erase(it->second);
        lru_list_.push_back(frame_id);
        lru_map_[frame_id] = std::prev(lru_list_.end());

    }
}


// replace page from the replacer list, a very first node (least recently used)
bool LRUReplacer::victim(frame_id_t* frame_id) {
    // all frames/pages are in used, pin_count > 0
    if(lru_list_.empty()) return false;

    // replace least recently used frame which is at the front
    *frame_id = lru_list_.front();
    lru_list_.pop_front();

    // replacer not any more track that frame 
    lru_map_.erase(*frame_id);

    return true;
}

