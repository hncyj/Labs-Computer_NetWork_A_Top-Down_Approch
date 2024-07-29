#include <iostream>
#include <unistd.h>
#include <cctype>
#include <vector>
#include <string>
#include <cstring>
#include <functional>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    std::cout << "\n----------- Server Start --------------" << std::endl;
    int server_socket_fd;
    struct sockaddr_in server_sock_addr, client_sock_addr;
    std::vector<char> buffer(BUFFER_SIZE);

    std::function<void(std::vector<char>&)> uppercase_msg = [](std::vector<char>& buffer) {
        for (char& ch : buffer) {
            if (std::isalpha(static_cast<unsigned char>(ch))) {
                ch = std::toupper(static_cast<unsigned char>(ch));
            } 
        }
    };

    if ((server_socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("create server socket failed.");
        exit(EXIT_FAILURE);
    }
    std::cout << "Create Server socket success." << std::endl;

    memset(&server_sock_addr, 0, sizeof(server_sock_addr));
    memset(&client_sock_addr, 0, sizeof(client_sock_addr));
    server_sock_addr.sin_family = AF_INET;
    server_sock_addr.sin_port = htons(PORT);
    server_sock_addr.sin_addr.s_addr = INADDR_ANY;

    socklen_t server_sock_addr_len = sizeof(server_sock_addr);
    if (bind(server_socket_fd, (struct sockaddr*) &server_sock_addr, server_sock_addr_len) < 0) {
        perror("bind server socket to its addr failed.");
        close(server_socket_fd);
        exit(EXIT_FAILURE);
    }
    std::cout << "Server socket bind success." << std::endl;

    std::cout << "Server socket start listening..." << std::endl;
    while (true) {
        buffer.clear();
        buffer.resize(BUFFER_SIZE);
        socklen_t client_sock_addr_len = sizeof(client_sock_addr);
        int msg_len = recvfrom(server_socket_fd, buffer.data(), buffer.size(), 0, (struct sockaddr*) &client_sock_addr, &client_sock_addr_len);
        if (msg_len <= 0) {
            perror("Recieve msg from client failed.");
            close(server_socket_fd);
            exit(EXIT_FAILURE);
        }
        buffer[msg_len] = '\0';
        std::cout << "Client msg: " <<std::string(buffer.begin(), buffer.begin() + msg_len) << std::endl;
        uppercase_msg(buffer);
        client_sock_addr_len = sizeof(client_sock_addr);
        if (sendto(server_socket_fd, buffer.data(), buffer.size(), 0, (struct sockaddr*) &client_sock_addr, client_sock_addr_len) < 0) {
            perror("Send Message failed.");
            close(server_socket_fd);
            exit(EXIT_FAILURE);
        }
        std::cout << "---------------------------" << std::endl;        
    }
    close(server_socket_fd);

    return 0;
}