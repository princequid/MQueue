#include "TopicManager.h"
#include <algorithm> // For std::remove

void TopicManager::add_subscriber(const std::string& topic, int client_fd) {
    std::lock_guard<std::mutex> lock(mutex_);
    topic_subscribers_[topic].push_back(client_fd);
}

void TopicManager::remove_subscriber(const std::string& topic, int client_fd) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto& subscribers = topic_subscribers_[topic];
    subscribers.erase(std::remove(subscribers.begin(), subscribers.end(), client_fd), subscribers.end());
}

std::vector<int> TopicManager::get_subscribers(const std::string& topic) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = topic_subscribers_.find(topic);
    return it != topic_subscribers_.end() ? it->second : std::vector<int>{};
}