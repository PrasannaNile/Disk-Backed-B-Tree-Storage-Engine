#include "storage/disk_manager.h"


// Constructor
DiskManager::DiskManager(const std::string& db_file) : file_name_{db_file} {
    // Open the file stream with explicit binary and I/O flags
    this->db_io_.open(db_file, std::ios::binary | std::ios::in | std::ios::out);

    // If the file doesn't exist, create it by truncating a new file
    if(!this->db_io_.is_open()) {
        this->db_io_.open(db_file, std::ios::binary | std::ios::trunc | std::ios::in | std::ios::out);

        // if unable to create a file, throw an error
        if(this->db_io_.is_open()) {
            std::runtime_error("Failed to create or open database file: " + db_file);
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
