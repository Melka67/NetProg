#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define ECHO_PORT 7
#define BUFFER_SIZE 1024
#define SERVER_IP "172.16.40.1"

class EchoClient {
private:
    int sockfd;
    struct sockaddr_in server_addr;
    
public:
    EchoClient() : sockfd(-1) {
        memset(&server_addr, 0, sizeof(server_addr));
    }
    
    bool initialize() {
        // Создание TCP сокета
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            std::cerr << "Ошибка создания сокета" << std::endl;
            return false;
        }
        
        // Настройка адреса сервера
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(ECHO_PORT);
        
        if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
            std::cerr << "Неверный адрес сервера" << std::endl;
            return false;
        }
        
        // Установка таймаута (5 секунд)
        struct timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        
        if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
            std::cerr << "Ошибка установки таймаута приема" << std::endl;
        }
        
        if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
            std::cerr << "Ошибка установки таймаута отправки" << std::endl;
        }
        
        return true;
    }
    
    bool connectToServer() {
        if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            std::cerr << "Ошибка подключения к серверу" << std::endl;
            return false;
        }
        
        std::cout << "Подключено к серверу " << SERVER_IP << ":" << ECHO_PORT << std::endl;
        return true;
    }
    
    void run() {
        std::string input;
        char buffer[BUFFER_SIZE];
        
        std::cout << "Введите сообщение для отправки (или 'quit' для выхода):" << std::endl;
        
        while (true) {
            std::cout << "> ";
            std::getline(std::cin, input);
            
            if (input == "quit") {
                break;
            }
            
            if (input.empty()) {
                continue;
            }
            
            // Отправка сообщения
            ssize_t sent_len = send(sockfd, input.c_str(), input.length(), 0);
            if (sent_len < 0) {
                std::cerr << "Ошибка отправки сообщения" << std::endl;
                break;
            }
            
            std::cout << "Отправлено: " << input << std::endl;
            
            // Получение эхо-ответа
            ssize_t recv_len = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
            if (recv_len < 0) {
                std::cerr << "Ошибка получения ответа" << std::endl;
                break;
            } else if (recv_len == 0) {
                std::cout << "Сервер закрыл соединение" << std::endl;
                break;
            }
            
            buffer[recv_len] = '\0';
            std::cout << "Получено: " << buffer << std::endl;
        }
    }
    
    ~EchoClient() {
        if (sockfd >= 0) {
            close(sockfd);
            std::cout << "Соединение закрыто" << std::endl;
        }
    }
};

int main() {
    std::cout << "=== TCP Echo Client ===" << std::endl;
    
    EchoClient client;
    
    if (!client.initialize()) {
        return 1;
    }
    
    if (!client.connectToServer()) {
        return 1;
    }
    
    client.run();
    
    return 0;
}