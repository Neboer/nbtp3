#pragma once

#include "SocketServer.h"
#include <winsock2.h>
#include <ws2tcpip.h>

class WinClientSocket : public UClientSocket {
public:
    void send(const std::string &msg) override;

    std::string recv(size_t length) override;

    void connect(std::string address, port_str port) override;

    void close() override;

    explicit WinClientSocket(SOCKET socket);

    explicit WinClientSocket();

    ~WinClientSocket();

private:
    SOCKET client_socket;
    bool init_with_socket;
};

class WinServerSocket : public UServerSocket {
public:
    WinServerSocket();

    WinClientSocket* accept() override;

    void bind_and_listen(std::string address, port_str port, int max_wait) override;

    void close() override;

    ~WinServerSocket();

private:
    SOCKET server_socket;
};
