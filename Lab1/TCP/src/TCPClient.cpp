#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define SIZE 1024

int main() {
    int client_sock_fd;
    struct sockaddr_in server_sock_addr;
    char buffer[SIZE];
    
    // create client socket
    if ((client_sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Create client socket failed." << std::endl;
        return -1;
    }
    
    memset(&server_sock_addr, 0, sizeof(server_sock_addr));
    server_sock_addr.sin_family = AF_INET;
    server_sock_addr.sin_port = htons(PORT);

    // turn server IP address to binary form
    const std::string server_IP = "127.0.0.1";
    if (inet_pton(AF_INET, server_IP.c_str() , &server_sock_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address / Address not supported." << std::endl;
        return -1;
    }

    // connect to server
    socklen_t server_sock_addr_len = sizeof(server_sock_addr);
    if (connect(client_sock_fd, (struct sockaddr*) &server_sock_addr, server_sock_addr_len) < 0) {
        std::cerr << "Connect failed." << std::endl;
        return -1;
    }

    std::cout << "Connetc to server ... done." << std::endl;
    while (true) {
        std::string message;
        std::cout << "Input message (Enter 'exit' to quit):";
        std::getline(std::cin, message);
        if (message == "exit") break;

        // send msg to server
        send(client_sock_fd, message.c_str(), message.size(), 0);
        memset(buffer, 0, SIZE);
        int read_len = read(client_sock_fd, buffer, SIZE - 1);
        if (read_len < 0) {
            std::cerr << "Read failed." << std::endl;
            return -1;
        }
        buffer[read_len] = '\0';
        std::cout << "Msg received from server: " << buffer << std::endl;
    }
    close(client_sock_fd);

    return 0;
}