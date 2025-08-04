#include "Broker.h"
#include "SocketServer.h"
#include <iostream>
#include <winsock2.h>

Broker::Broker(const std::string& storage_path)
    : persistence_(storage_path) {}

Broker::~Broker() {}

void Broker::start(int port) {
    SocketServer server([this](int client_fd, const std::string& data) {
        std::cout << "Broker processing data from client " << client_fd << ": " << data << std::endl;
        if (data.find("PUBLISH") == 0) {
            size_t topic_start = data.find(' ', 8); // After "PUBLISH "
            if (topic_start == std::string::npos) {
                std::cerr << "Invalid PUBLISH command: no topic found" << std::endl;
                return;
            }
            std::string topic = data.substr(8, topic_start - 8);
            std::string message = data.substr(topic_start + 1); // Take everything after topic
            if (message.empty()) {
                std::cerr << "Invalid PUBLISH command: no message found" << std::endl;
                return;
            }
            std::cout << "Parsed topic: " << topic << ", message: " << message << std::endl;
            publish(topic, message);
        } else if (data.find("SUBSCRIBE") == 0) {
            std::string topic = data.substr(10);
            if (topic.empty()) {
                std::cerr << "Invalid SUBSCRIBE command: no topic found" << std::endl;
                return;
            }
            subscribe(client_fd, topic);
        }
    });
    server.start(port);
}

void Broker::publish(const std::string& topic, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::string message_id = std::to_string(std::hash<std::string>{}(message + topic));
    persistence_.store_message(message_id, topic, message);
    auto subscribers = topic_manager_.get_subscribers(topic);
    std::cout << "Publishing to topic " << topic << ", subscribers: " << subscribers.size() << std::endl;
    for (int client_fd : subscribers) {
        std::string data = "MESSAGE " + topic + " " + message;
        if (send(client_fd, data.c_str(), data.size(), 0) == SOCKET_ERROR) {
            std::cerr << "Failed to send to client " << client_fd << ": " << WSAGetLastError() << std::endl;
            continue;
        }
        std::cout << "Sent to client " << client_fd << ": " << data << std::endl;
    }
}

void Broker::subscribe(int client_fd, const std::string& topic) {
    std::lock_guard<std::mutex> lock(mutex_);
    topic_manager_.add_subscriber(topic, client_fd);
    std::cout << "Subscribed client " << client_fd << " to topic " << topic << std::endl;
}

void Broker::unsubscribe(int client_fd, const std::string& topic) {
    std::lock_guard<std::mutex> lock(mutex_);
    topic_manager_.remove_subscriber(topic, client_fd);
}

void Broker::acknowledge(int client_fd, const std::string& message_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    persistence_.mark_acknowledged(message_id);
}