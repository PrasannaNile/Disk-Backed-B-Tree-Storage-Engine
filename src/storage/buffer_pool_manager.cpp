#include "storage/buffer_pool_manager.h"


// write the page back to persistent memory if dirty bit is set
bool BufferPoolManager::flushPage(page_id_t page_id) {
    // locate this page inside the RAM (get the frame id)
    auto it = page_table_.find(page_id);

    // page does not exist in RAM currently
    if(it == page_table_.end()) return false;

    Page& page = frames_[it->second];

    // if page is already persist (dirty bit not set)
    if(!page.IsDirty()) return true;

    // if page is dirty, then write back to persistent memory
    disk_manager_->WritePage(page_id, page.GetData());
    page.SetDirty(false);

    return true;
}


// query is done using the page (unpin page and send to replacer if necessary)
bool BufferPoolManager::UnpinPage(page_id_t page_id, bool is_dirty) {
    auto it = page_table_.find(page_id);

    // page not in RAM
    if(it == page_table_.end()) return false;

    Page& page = frames_[it->second];

    // trying to unpin tha page which is no longer hold by any query
    if(page.GetPinCount() <= 0) return false;
    page.DecreasePinCount();

    // if page is modified during the process, update the dirty bit of page
    if(is_dirty == true) page.SetDirty(true);

    // if pin_count == 0 then send to replacer
    if(page.GetPinCount() == 0) replacer.unpin(it->second);
    return true;
}


Page* BufferPoolManager::FetchPage(page_id_t page_id) {
    auto it = page_table_.find(page_id);
    
    // cache hit
    if(it != page_table_.end()) {

        Page& page = frames_[it->second];

        // tell system some query is using the page
        page.IncreasePinCount();
        replacer.pin(it->second);

        return &page;

    }

    // cache miss: page not found inside the RAM
    frame_id_t frame_id = -1;

    // if frame is empty
    if(!free_list_.empty()) {
        frame_id = free_list_.back();
        free_list_.pop_back();
    }
    // try replacer to get a victim frame
    else if(replacer.victim(&frame_id)) {
        Page& victim_page = frames_[frame_id];

        // write back if evicted page is dirty
        if(victim_page.IsDirty()) {
            disk_manager_->WritePage(victim_page.GetPageId(), victim_page.GetData());
            victim_page.SetDirty(false);
        }

        page_table_.erase(victim_page.GetPageId());
    }

    else return nullptr;

    // Load the New Page into the picked Frame
    Page& page = frames_[frame_id];

    page.ResetMemory();
    disk_manager_->ReadPage(page_id, page.GetData());

    // Set up our fresh Page metadata
    page.SetPageId(page_id);
    page.IncreasePinCount();
    page.SetDirty(false);

    // Update tracking structures
    page_table_[page_id] = frame_id;
    replacer.pin(frame_id);

    return &page;
}