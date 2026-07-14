#include "storage/buffer_pool_manager.h"



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