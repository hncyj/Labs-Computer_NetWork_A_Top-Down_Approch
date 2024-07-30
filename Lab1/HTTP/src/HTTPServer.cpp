#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fstream>
#include <sstream>
#include <cstring>

#define PORT 9090  
#define BUFFER_SIZE 1024

int main() {
    int server_socket_fd, connect_socket_fd;
    struct sockaddr_in server_sock_addr, connect_sock_addr;
    char buffer[BUFFER_SIZE];

    if ((server_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Create server socket failed");
        return 1;
    }
    std::cout << "Server socket created successfully." << std::endl;

    int opt = 1;
    if (setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Setsockopt failed");
        close(server_socket_fd);
        return 1;
    }

    memset(&server_sock_addr, 0, sizeof(server_sock_addr));  // 清零
    server_sock_addr.sin_family = AF_INET;
    server_sock_addr.sin_addr.s_addr = INADDR_ANY;  // 接受任何地址
    server_sock_addr.sin_port = htons(PORT);

    if (bind(server_socket_fd, (struct sockaddr *)&server_sock_addr, sizeof(server_sock_addr)) < 0) {
        perror("Bind failed");
        close(server_socket_fd);
        return 1;
    }

    if (listen(server_socket_fd, 5) < 0) {
        perror("Listen failed");
        close(server_socket_fd);
        return 1;
    }
    std::cout << "Listening on port " << PORT << "..." << std::endl;

    while (true) {
        socklen_t len = sizeof(connect_sock_addr);
        connect_socket_fd = accept(server_socket_fd, (struct sockaddr *)&connect_sock_addr, &len);
        if (connect_socket_fd < 0) {
            perror("Accept failed");
            continue;
        }

        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = read(connect_socket_fd, buffer, BUFFER_SIZE - 1);
        if (bytes_read < 0) {
            perror("Error reading from socket");
            close(connect_socket_fd);
            continue;
        }

        std::string filename = "cs50's.html";
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::string response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
            send(connect_socket_fd, response.c_str(), response.length(), 0);
        } else {
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            std::stringstream response;
            response << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n";
            response << "Content-Length: " << content.size() << "\r\n\r\n";
            response << content;
            send(connect_socket_fd, response.str().c_str(), response.str().length(), 0);
            file.close();
        }
        close(connect_socket_fd);
    }
    close(server_socket_fd);
    
    return 0;
}