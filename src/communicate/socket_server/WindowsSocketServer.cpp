#include "WindowsSocketServer.h"
#include <winerror.h>
#include <iostream>

constexpr addrinfo normal_tcp_server{AI_PASSIVE, AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, nullptr, nullptr, nullptr};
constexpr addrinfo normal_tcp_client{0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, nullptr, nullptr, nullptr};

void report_error() {
    constexpr int max_message_length = 256;
    auto ctext = new char[max_message_length];
    DWORD error = WSAGetLastError();
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, 0, ctext, max_message_length, NULL);
    throw std::runtime_error(ctext);
}

#define ASSERT_ZERO if(iResult != 0) report_error()
#define ASSERT_NOT_SOCKET_ERROR if(iResult == SOCKET_ERROR) report_error()

WinServerSocket::WinServerSocket() : UServerSocket{} {
    WSADATA wsaData;
    auto iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    ASSERT_ZERO;
    server_socket = INVALID_SOCKET;
    status = CLOSED;
}

void WinServerSocket::bind_and_listen(std::string address, port_str port, int max_wait) {
    addrinfo *result_list{};
    auto iResult = getaddrinfo((const char *) address.c_str(), port, (&normal_tcp_server), &result_list);
    ASSERT_ZERO;
    server_socket = socket(result_list->ai_family, result_list->ai_socktype, result_list->ai_protocol);
    if (server_socket == INVALID_SOCKET) report_error();
    iResult = bind(server_socket, result_list->ai_addr, (int) result_list->ai_addrlen);
    ASSERT_ZERO;
    freeaddrinfo(result_list);
    iResult = listen(server_socket, max_wait);
    ASSERT_ZERO;
    status = LISTENING;
}


WinClientSocket *WinServerSocket::accept() {
    return new WinClientSocket(::accept(server_socket, nullptr, nullptr));
}

void WinServerSocket::close() {
    if (status == LISTENING) closesocket(server_socket);
    status = CLOSED;
}


WinServerSocket::~WinServerSocket() {
    WinServerSocket::close();
    WSACleanup();
}

void WinClientSocket::send(const std::string &msg) {
    auto iResult = ::send(client_socket, msg.c_str(), msg.length(), 0);
    ASSERT_NOT_SOCKET_ERROR;
}

std::string WinClientSocket::recv(size_t length) {
    char buffer[length];
    auto iResult = ::recv(client_socket, buffer, length, 0);
    ASSERT_NOT_SOCKET_ERROR;
    else return {buffer, length};
}

void WinClientSocket::close() {
    closesocket(client_socket);
}

WinClientSocket::WinClientSocket(SOCKET socket) : client_socket(socket), init_with_socket(true) {

}

WinClientSocket::~WinClientSocket() {
    if (status == CONNECTED) WinClientSocket::close();
    if (!init_with_socket) WSACleanup();
}

SOCKET connect_until_successful(const addrinfo* result_chain) {
    auto s = INVALID_SOCKET;
    if (!result_chain) throw ConnectionFailed{};
    else {
        s = socket(result_chain->ai_family, result_chain->ai_socktype, result_chain->ai_protocol);
        if (s == INVALID_SOCKET) report_error();
        else {
            if (::connect(s, result_chain->ai_addr, (int) result_chain->ai_addrlen) == SOCKET_ERROR)
                connect_until_successful(result_chain->ai_next);
            else return s;
        }
    }
}

void WinClientSocket::connect(std::string address, port_str port) {
    addrinfo *result_list{};
    auto iResult = getaddrinfo((const char *) address.c_str(), port, &normal_tcp_client, &result_list);
    ASSERT_ZERO;
    client_socket = connect_until_successful(result_list);
    freeaddrinfo(result_list);
    status = CONNECTED;
}

WinClientSocket::WinClientSocket() : UClientSocket(), init_with_socket(false) {
    WSADATA wsaData;
    auto iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    ASSERT_ZERO;
    client_socket = INVALID_SOCKET;
    status = CLOSED;
}
