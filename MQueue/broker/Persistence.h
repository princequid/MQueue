#pragma once
#include <string>

class Persistence {
public:
    Persistence(const std::string& storage_path);
    void store_message(const std::string& message_id, const std::string& topic, const std::string& message);
    void mark_acknowledged(const std::string& message_id);

private:
    std::string storage_path_;
};