#include "WindowsSocketServer.h"
#include<iostream>

void tester(UServerSocket& ss) {
    ss.bind_and_listen("localhost", "27015", 2);
    auto* ac = ss.accept();
    ac->send("hello!");
    std::cout << ac->recv(5);
    ac->close();
    ss.close();
}

void test_connect(UClientSocket& cs) {
    cs.connect("localhost", "27015");
    cs.send("wtf!");
}

int main() {
    WinClientSocket wss{};
    test_connect(wss);
}