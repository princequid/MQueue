#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>

class TopicManager {
public:
    // Add a subscriber to a topic
    void add_subscriber(const std::string& topic, int client_fd);

    // Remove a subscriber from a topic
    void remove_subscriber(const std::string& topic, int client_fd);

    // Get all subscribers for a topic
    std::vector<int> get_subscribers(const std::string& topic) const;

private:
    std::unordered_map<std::string, std::vector<int>> topic_subscribers_;
    mutable std::mutex mutex_;
};