#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>
#include "TopicManager.h"
#include "Persistence.h"

class Broker {
public:
    Broker(const std::string& storage_path);
    ~Broker();

    // Start the broker, initialize socket server
    void start(int port);

    // Publish a message to a topic
    void publish(const std::string& topic, const std::string& message);

    // Subscribe a client to a topic
    void subscribe(int client_fd, const std::string& topic);

    // Unsubscribe a client from a topic
    void unsubscribe(int client_fd, const std::string& topic);

    // Handle client acknowledgment
    void acknowledge(int client_fd, const std::string& message_id);

private:
    TopicManager topic_manager_;
    Persistence persistence_;
    std::mutex mutex_;
};