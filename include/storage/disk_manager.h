/* -------------------------------------------
# Interacting directly with hard drive 
# writing 4KB data and reading 4KB chunk of data
-------------------------------------------- */

#pragma once

#include "storage/page.h"

#include <string>
#include <fstream>

class DiskManager {
private:
    std::fstream db_io_;    // The actual file stream object for binary I/O
    std::string file_name_; // Stores the path to the database file
    uint64_t num_pages{0};
public:
    // Constructor: Opens the physical file on your disk
    explicit DiskManager(const std::string& db_file);

    // Cleanly closes the file when the database stops
    ~DiskManager();

    // Write API: Takes a 4KB array from RAM and writes it to a specific page on disk
    void WritePage(const page_id_t page_id, const char* page_data);

    // Read API: Finds a page on disk and copies its 4KB content into a RAM buffer
    void ReadPage(const page_id_t page_id, char* page_data);

    page_id_t AllocatePage() { return ++num_pages; }
};
