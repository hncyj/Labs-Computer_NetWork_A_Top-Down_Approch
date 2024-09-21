#include <iostream>
#include <unistd.h>
#include <cctype>
#include <fcntl.h>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <functional>

#define PORT 8080
#define SIZE 1024

int main() {
    std::cout << "\n------------- Server Start -------------" << std::endl;;
    int server_sock_fd, new_sock_fd;
    struct sockaddr_in server_sock_addr;
    char buffer[SIZE];

    // uppercase func
    std::function<void(char*, int)> uppercase_msg = [](char* buffer, int size) {
        for (int i = 0; i < size; ++i) {
            if (std::isalpha(static_cast<unsigned char>(buffer[i]))) {
                buffer[i] = std::toupper(static_cast<unsigned char>(buffer[i]));
            }
        }
    }; 
    
    // create welcome socket
    if ((server_sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("server's welcome socket create failed.");
        exit(EXIT_FAILURE);
    }
    std::cout << "Server welcome socket create success." << std::endl;

    // set welcome socket
    int opt = 1;
    socklen_t opt_size = sizeof(opt);
    if (setsockopt(server_sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, opt_size) < 0) {
        perror("set welcome socket's opt failed.");
        exit(EXIT_FAILURE);
    }
    std::cout << "Set welcome socket opt success." << std::endl;

    // int flags = fcntl(server_sock_fd, F_GETFL, 0);
    // if (flags < 0) {
    //     perror("fcntl F_GETFL failed.");
    //     exit(EXIT_FAILURE);
    // }
    // if (fcntl(server_sock_fd, F_SETFL, flags | O_NONBLOCK) < 0) {
    //     perror("fcntl F_SETFL failed to set non-blocking.");
    //     exit(EXIT_FAILURE);
    // }

    // set welcom socket addr info
    server_sock_addr.sin_family = AF_INET;
    server_sock_addr.sin_port = htons(PORT);
    server_sock_addr.sin_addr.s_addr = INADDR_ANY;

    // bind welcome socket to its addr info
    socklen_t server_sock_addr_len = sizeof(server_sock_addr);
    if (bind(server_sock_fd, (struct sockaddr*) &server_sock_addr, server_sock_addr_len) < 0) {
        perror("bind welcome socket to its addr info failed.");
        exit(EXIT_FAILURE);
    }
    std::cout << "bind welcome socket to its addr info success." << std::endl;

    // listen connection
    // queue size = 1;
    if (listen(server_sock_fd, 1) < 0) {
        perror("listen connection failed.");
        exit(EXIT_FAILURE);
    }
    std::cout << "Start Listening..." << std::endl;
    while (true) {
        // accept new connection
        if ((new_sock_fd = accept(server_sock_fd, (struct sockaddr*) &server_sock_addr, &server_sock_addr_len)) < 0) {
            perror("Accept connection failed.");
            exit(EXIT_FAILURE);
        }
        std::cout << "Connection accept." << std::endl;
        while (true) {
            memset(buffer, 0, SIZE - 1);
            int read_len = read(new_sock_fd, buffer, SIZE - 1);
            if (read_len <= 0) {
                std::cerr << "Reading from socket failed." << std::endl;
                break;
            }
            buffer[read_len] = '\0';
            std::cout << "Message from client: " << buffer << std::endl;
            uppercase_msg(buffer, read_len);
            send(new_sock_fd, buffer, read_len, 0);
        }
        close(new_sock_fd);
        std::cout << "Connection Closed." << std::endl;
    }
    close(server_sock_fd);

    return 0;
}