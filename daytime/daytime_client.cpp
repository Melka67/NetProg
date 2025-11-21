#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    SOCKET sock;
    sockaddr_in server_addr;
    char buffer[256];
    int bytes_received;
    
    // Инициализация Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
        return 1;
    }
    
    // Создание сокета
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }
    
    // Настройка адреса сервера
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(13); // Порт daytime службы
    server_addr.sin_addr.s_addr = inet_addr("172.16.40.1");
    
    // Проверка корректности IP-адреса
    if (server_addr.sin_addr.s_addr == INADDR_NONE) {
        std::cerr << "Invalid IP address" << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    
    std::cout << "Sending request to daytime server..." << std::endl;
    
    // Отправка пустого пакета для получения времени
    if (sendto(sock, "", 0, 0, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    
    // Получение ответа
    int server_len = sizeof(server_addr);
    bytes_received = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, 
                             (sockaddr*)&server_addr, &server_len);
    
    if (bytes_received == SOCKET_ERROR) {
        std::cerr << "Receive failed: " << WSAGetLastError() << std::endl;
    } else {
        buffer[bytes_received] = '\0';
        std::cout << "Daytime from server: " << buffer;
    }
    
    // Завершение работы
    closesocket(sock);
    WSACleanup();
    return 0;
}