#pragma once

#include <fmt/core.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>

class App {
   public:
    App();
    ~App();

    std::string GetInput();
    bool SendMessageToServer(SOCKET client_socket, const char* message);
    std::string ReceiveMessage(SOCKET client_socket);
    void GetInputAndSendMessage(SOCKET client_socket);

   private:
    int m_server_port;
    PCSTR m_server_ip;
};