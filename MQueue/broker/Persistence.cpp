#include "Persistence.h"
#include <fstream>
#include <iostream>

Persistence::Persistence(const std::string& storage_path) : storage_path_(storage_path) {}

void Persistence::store_message(const std::string& message_id, const std::string& topic, const std::string& message) {
    std::ofstream file(storage_path_ + "/" + message_id + ".txt");
    if (file.is_open()) {
        file << topic << "\n" << message << "\n";
        file.close();
    } else {
        std::cerr << "Failed to store message " << message_id << std::endl;
    }
}

void Persistence::mark_acknowledged(const std::string& message_id) {
    // In a real implementation, mark message as ACK'd or remove it
    std::cout << "Message " << message_id << " acknowledged" << std::endl;
}