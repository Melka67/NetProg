#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define DAYTIME_PORT 13
#define BUFFER_SIZE 1024
#define SERVER_IP "172.16.40.1"

class DaytimeClient {
private:
    int sockfd;
    struct sockaddr_in server_addr;
    
public:
    DaytimeClient() : sockfd(-1) {
        memset(&server_addr, 0, sizeof(server_addr));
    }
    
    bool initialize() {
        // Создание UDP сокета
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0) {
            std::cerr << "Ошибка создания сокета" << std::endl;
            return false;
        }
        
        // Настройка адреса сервера
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(DAYTIME_PORT);
        
        if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
            std::cerr << "Неверный адрес сервера" << std::endl;
            return false;
        }
        
        // Установка таймаута на получение данных (5 секунд)
        struct timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        
        if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
            std::cerr << "Ошибка установки таймаута" << std::endl;
        }
        
        return true;
    }
    
    bool getTime() {
        // Отправка пустого датаграммы для запроса времени
        if (sendto(sockfd, "", 0, 0, 
                  (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            std::cerr << "Ошибка отправки запроса" << std::endl;
            return false;
        }
        
        std::cout << "Запрос отправлен на сервер " << SERVER_IP << ":" << DAYTIME_PORT << std::endl;
        
        // Получение ответа
        char buffer[BUFFER_SIZE];
        socklen_t addr_len = sizeof(server_addr);
        
        ssize_t recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                                   (struct sockaddr*)&server_addr, &addr_len);
        
        if (recv_len < 0) {
            std::cerr << "Ошибка получения ответа или таймаут" << std::endl;
            return false;
        }
        
        buffer[recv_len] = '\0';
        std::cout << "Текущее время: " << buffer;
        
        return true;
    }
    
    ~DaytimeClient() {
        if (sockfd >= 0) {
            close(sockfd);
        }
    }
};

int main() {
    std::cout << "=== UDP Daytime Client ===" << std::endl;
    
    DaytimeClient client;
    
    if (!client.initialize()) {
        return 1;
    }
    
    if (!client.getTime()) {
        return 1;
    }
    
    return 0;
}