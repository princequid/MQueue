#include "SocketServer.h"
#include <iostream>
#include <vector>
#include <thread>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

SocketServer::SocketServer(MessageHandler handler) : handler_(handler), server_fd_(-1) {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
        exit(1);
    }
#endif
}

SocketServer::~SocketServer() {
#ifdef _WIN32
    if (server_fd_ != INVALID_SOCKET) closesocket(server_fd_);
    WSACleanup();
#else
    if (server_fd_ != -1) close(server_fd_);
#endif
    for (auto& thread : client_threads_) {
        if (thread.joinable()) thread.join();
    }
}

void SocketServer::start(int port) {
#ifdef _WIN32
    server_fd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd_ == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        return;
    }
#else
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ == -1) {
        std::cerr << "Socket creation failed" << std::endl;
        return;
    }
#endif

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(server_fd_, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
#ifdef _WIN32
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        closesocket(server_fd_);
#else
        std::cerr << "Bind failed" << std::endl;
        close(server_fd_);
#endif
        return;
    }

    if (listen(server_fd_, 10) == -1) {
#ifdef _WIN32
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        closesocket(server_fd_);
#else
        std::cerr << "Listen failed" << std::endl;
        close(server_fd_);
#endif
        return;
    }

    std::cout << "Server listening on port " << port << std::endl;

    while (true) {
#ifdef _WIN32
        SOCKET client_fd = accept(server_fd_, nullptr, nullptr);
        if (client_fd == INVALID_SOCKET) {
            std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
            continue;
        }
#else
        int client_fd = accept(server_fd_, nullptr, nullptr);
        if (client_fd == -1) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }
#endif
        std::cout << "Accepted client with fd " << client_fd << std::endl;
        client_threads_.emplace_back(&SocketServer::handle_client, this, client_fd);
    }
}

void SocketServer::handle_client(int client_fd) {
    char buffer[1024];
    while (true) {
        ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_read <= 0) {
            std::cerr << "Client " << client_fd << " disconnected or error: "
                      << (bytes_read == 0 ? "connection closed" : std::to_string(WSAGetLastError())) << std::endl;
#ifdef _WIN32
            closesocket(client_fd);
#else
            close(client_fd);
#endif
            break;
        }
        buffer[bytes_read] = '\0';
        std::cout << "Received from client " << client_fd << ": " << buffer << std::endl;
        handler_(client_fd, std::string(buffer));
    }
}