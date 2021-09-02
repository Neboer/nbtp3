#pragma once

#include <memory>
#include <stdexcept>

class ConnectAlreadyClosed : std::exception {
}; // read/write to a connection which is already closed.
class PortAlreadyInUsed : std::exception {
}; // bind to a port which is already in use
class ConnectionFailed : std::exception {}; // just can't connect to the remote host.
class OtherError : std::exception {
};

typedef const char port_str[6];

// blocking socket.
class UClientSocket {
public:
    enum client_status {
        CONNECTED,
        CLOSED
    } status = CLOSED;

    virtual void connect(std::string address, port_str port) = 0;

    virtual void send(const std::string &msg) = 0;

    virtual std::string recv(size_t length) = 0;

    virtual void close() = 0;

    UClientSocket(const UClientSocket&) = delete;
    UClientSocket() = default;
};

class UServerSocket {
public:
    enum server_status {
        LISTENING,
        CLOSED
    } status = CLOSED;

    virtual void bind_and_listen(std::string address, port_str port, int max_wait) = 0;

    virtual UClientSocket* accept() = 0;

    virtual void close() = 0;

    UServerSocket(const UServerSocket&) = delete;
    UServerSocket() = default;
};