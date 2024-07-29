#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <cstring>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int client_socket_fd;
    struct sockaddr_in server_sock_addr;
    std::vector<char> buffer(BUFFER_SIZE);

    if ((client_socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Create client socket failed.");
        exit(EXIT_FAILURE);
    }
    std::cout << "Create client socket success." << std::endl;

    memset(&server_sock_addr, 0, sizeof(server_sock_addr));
    server_sock_addr.sin_family = AF_INET;
    server_sock_addr.sin_port = htons(PORT);

    std::string server_IP = "127.0.0.1";
    if (inet_pton(AF_INET, server_IP.c_str(), &server_sock_addr.sin_addr) <= 0) {
        std::cerr << "Invalid Address / Address not supported." << std::endl;
        close(client_socket_fd);
        return -1;
    }

    while (true) {
        std::string msg;
        std::cout << "Enter message (Enter 'exit' to quit): ";
        std::getline(std::cin, msg);
        socklen_t server_sock_addr_len = sizeof(server_sock_addr);
        if (sendto(client_socket_fd, msg.c_str(), msg.size(), 0, (struct sockaddr*) &server_sock_addr, server_sock_addr_len) < 0) {
            std::cerr << "send failed." << std::endl;
            break;
        }
        buffer.clear();
        buffer.resize(BUFFER_SIZE);
        int msg_len = recvfrom(client_socket_fd, buffer.data(), BUFFER_SIZE, 0, (struct sockaddr*) &server_sock_addr, &server_sock_addr_len);
        if (msg_len < 0) {
            std::cerr << "Received from server failed." << std::endl;
            close(client_socket_fd);
            return -1;
        }
        buffer[msg_len] = '\0';
        std::cout << "Msg from server: " << std::string(buffer.begin(), buffer.begin() + msg_len) << std::endl;
        if (msg == "exit") break;
        std::cout << "--------------------------------" << std::endl;
    }
    close(client_socket_fd);
    std::cout << "Connection closed." << std::endl;
 
    return 0;
}

