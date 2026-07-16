#include "storage/disk_manager.h"


// Constructor
DiskManager::DiskManager(const std::string& db_file) : file_name_{db_file} {
    // Open the file stream with explicit binary and I/O flags
    this->db_io_.open(db_file, std::ios::binary | std::ios::in | std::ios::out);

    // If the file doesn't exist, create it by truncating a new file (creates a fresh new file)
    if(!this->db_io_.is_open()) {
        // fail bit is set as first open fails
        this->db_io_.clear();
        this->db_io_.open(db_file, std::ios::binary | std::ios::trunc | std::ios::in | std::ios::out);

        // if unable to create a file, throw an error
        if(!this->db_io_.is_open()) {
            throw std::runtime_error("Failed to create or open database file: " + db_file);
        }
    }
}


// destructor
DiskManager::~DiskManager() {
    // close file if open
    if(this->db_io_.is_open()) {
        this->db_io_.close();
    }
}


// Write Page
void DiskManager::WritePage(const page_id_t page_id, const char* page_data) {
    // Calculate the exact byte offset in the file
    uint64_t byte_offset = page_id * PAGE_SIZE;

    // Move the file write pointer to the calculated offset
    this->db_io_.seekp(byte_offset);

    // Write exactly 4096 bytes from the memory buffer to the file
    this->db_io_.write(page_data, PAGE_SIZE);

    // Force the operating system to flush its internal cache out to the physical disk immediately
    this->db_io_.flush();
}


// Read Page
void DiskManager::ReadPage(const page_id_t page_id, char* page_data) {
    uint64_t byte_offset = page_id * PAGE_SIZE;

    this->db_io_.seekp(byte_offset);

    this->db_io_.read(page_data, PAGE_SIZE);
}