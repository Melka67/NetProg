#include <iostream>
#include <string>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    SOCKET client_socket;
    sockaddr_in server_addr;
    std::string message;
    char buffer[1024];
    int bytes_received;
    
    // Инициализация Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
        return 1;
    }
    
    // Создание сокета
    client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }
    
    // Настройка адреса сервера
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(7); // Порт echo службы
    server_addr.sin_addr.s_addr = inet_addr("172.16.40.1");
    
    // Проверка корректности IP-адреса
    if (server_addr.sin_addr.s_addr == INADDR_NONE) {
        std::cerr << "Invalid IP address" << std::endl;
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }
    
    // Подключение к серверу
    std::cout << "Connecting to echo server..." << std::endl;
    if (connect(client_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed: " << WSAGetLastError() << std::endl;
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }
    
    std::cout << "Connected to echo server successfully!" << std::endl;
    std::cout << "Type messages to send (type 'quit' to exit):" << std::endl;
    
    // Обмен сообщениями
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, message);
        
        if (message == "quit") {
            break;
        }
        
        // Отправка сообщения
        if (send(client_socket, message.c_str(), message.length(), 0) == SOCKET_ERROR) {
            std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
            break;
        }
        
        // Получение ответа
        bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received == SOCKET_ERROR) {
            std::cerr << "Receive failed: " << WSAGetLastError() << std::endl;
            break;
        } else if (bytes_received == 0) {
            std::cout << "Server closed connection" << std::endl;
            break;
        } else {
            buffer[bytes_received] = '\0';
            std::cout << "Echo: " << buffer << std::endl;
        }
    }
    
    // Завершение работы
    closesocket(client_socket);
    WSACleanup();
    std::cout << "Connection closed." << std::endl;
    return 0;
}