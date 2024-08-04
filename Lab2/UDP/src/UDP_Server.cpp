#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>

#include <iostream>
#include <random>
#include <cstring>

#define PORT 8080
#define SIZE 2048

void capitalize(char* buffer, int size) {
    for (int i = 0; i < size; ++i) {
        buffer[i] = std::toupper(static_cast<unsigned char>(buffer[i]));
    }
}

int main() {
    int server_socket_fd;
    struct sockaddr_in server_sock_addr, client_sock_addr;
    char buffer[SIZE];
    
    if ((server_socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("create server socket failed.\n");
        exit(EXIT_FAILURE);
    }
    std::cout << "server socket create success." << std::endl;

    memset(&server_sock_addr, 0, sizeof(server_sock_addr));
    memset(&client_sock_addr, 0, sizeof(client_sock_addr));
    server_sock_addr.sin_family = AF_INET;
    server_sock_addr.sin_addr.s_addr = INADDR_ANY;
    server_sock_addr.sin_port = htons(PORT);

    if (bind(server_socket_fd, (struct sockaddr*) &server_sock_addr, sizeof(server_sock_addr)) < 0) {
        perror("bind server socket to its addr failed.");
        close(server_socket_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "start listening ..." << std::endl;
    // create random number from [1, 10];
    std::random_device rd;
    std::mt19937 generate_func(rd()); // Mersenne Twister
    std::uniform_int_distribution<> dis(1, 10);
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int random_num = dis(generate_func);

        socklen_t len = sizeof(client_sock_addr);
        int msg_len = recvfrom(server_socket_fd, buffer, SIZE - 1, 0, (struct sockaddr*) &client_sock_addr, &len);
        if (msg_len <= 0) {
            perror("Recieve msg from client failed.");
            close(server_socket_fd);
            exit(EXIT_FAILURE);
        }
        buffer[msg_len] = '\0';
        ::capitalize(buffer, msg_len + 1);
        
        // simulate drop
        if (random_num < 4) {
            continue;
        }
        if (sendto(server_socket_fd, buffer, msg_len + 1, 0, (struct sockaddr*) &client_sock_addr, len) < 0) {
            perror("Send Message failed.");
            close(server_socket_fd);
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}