#include "app.h"

App::App() {
    m_server_port = 8080;
    m_server_ip = "127.0.0.1";
    fmt::print("Client starting...\n");

    // create a socket
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        fmt::print("WSAStartup failed: {}\n", result);
        return;
    }

    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket == INVALID_SOCKET) {
        fmt::print("Error at socket(): {}\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(m_server_port);
    inet_pton(AF_INET, m_server_ip, &server_address.sin_addr);

    // connect to server
    result = connect(client_socket, (SOCKADDR*)&server_address, sizeof(server_address));
    if (result == SOCKET_ERROR) {
        fmt::print("Failed to connect to server: {}\n", WSAGetLastError());
        closesocket(client_socket);
        WSACleanup();
        return;
    }

    fmt::print("Connected to server\n");

    std::thread send_thread(&App::GetInputAndSendMessage, this, client_socket);
    send_thread.detach();
    std::thread receive_thread(&App::ReceiveMessage, this, client_socket);
    receive_thread.detach();
    // std::string received_message = ReceiveMessage(client_socket);
    // fmt::print("Received message: {}\n", received_message);

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    // cleanup
    closesocket(client_socket);
    WSACleanup();
}

void App::GetInputAndSendMessage(SOCKET client_socket) {
    while (true) {
        std::string message = GetInput();
        const char* message_cstr = message.c_str();
        // const char* message = "Hello from client";

        // send data
        bool sent = SendMessageToServer(client_socket, message_cstr);
        if (!sent) {
            fmt::print("Failed to send data\n");
        } else {
            fmt::print("Data sent\n");
        }
    }
}

bool App::SendMessageToServer(SOCKET client_socket, const char* message) {
    int result = send(client_socket, message, strlen(message), 0);
    if (result == SOCKET_ERROR) {
        fmt::print("Failed to send data: {}\n", WSAGetLastError());
        return false;
    }

    return true;
}

std::string App::ReceiveMessage(SOCKET client_socket) {
    while (true) {
        char buffer[1024];
        int result = recv(client_socket, buffer, sizeof(buffer), 0);
        if (result == SOCKET_ERROR) {
            fmt::print("Failed to receive data: {}\n", WSAGetLastError());
            return "";
        }

        buffer[result] = '\0';
        std::string message = std::string(buffer);
        fmt::print("Received message: {}\n", message);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return "";
}

std::string App::GetInput() {
    fmt::print("Enter a message:\n");
    std::string message;
    std::getline(std::cin, message);

    return message;
}

App::~App() { fmt::print("Client shutting down...\n"); }

int main() {
    App* app = new App;
    return 0;
}