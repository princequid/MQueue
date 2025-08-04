#pragma once
#include <functional>
#include <string>
#include <thread>

class SocketServer {
public:
    using MessageHandler = std::function<void(int, const std::string&)>;

    SocketServer(MessageHandler handler);
    ~SocketServer();

    void start(int port);

private:
    void handle_client(int client_fd);

    MessageHandler handler_;
    int server_fd_;
    std::vector<std::thread> client_threads_;
};