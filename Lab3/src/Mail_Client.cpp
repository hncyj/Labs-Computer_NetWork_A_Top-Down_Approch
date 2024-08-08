#include <iostream>
#include <string>
#include <cstring>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

#define PORT 587
#define SIZE 1024

void sendCommand(int client_socket_fd, const std::string& command, const std::string& response) {
    char buffer[SIZE];
    if (send(client_socket_fd, command.c_str(), command.size(), 0) < 0) {
        std::cerr << "send failed." << std::endl;
        close(client_socket_fd);
        exit(EXIT_FAILURE);
    }
    int len = recv(client_socket_fd, buffer, SIZE - 1, 0);
    if (len <= 0) {
        std::cerr << "receive failed." << std::endl;
        close(client_socket_fd);
        exit(EXIT_FAILURE);
    }
    buffer[len] = '\0';
    if (strncmp(buffer, response.c_str(), 3) != 0) {
        std::cerr << "reply not received from server." << std::endl;
        close(client_socket_fd);
        exit(EXIT_FAILURE);
    }
    std::cout << buffer << std::endl;
}

int main() {
    // get mail server's host by name
    const std::string mail_server_domain = "smtp.gmail.com";
    struct hostent* host_ptr = gethostbyname(mail_server_domain.c_str());
    if (host_ptr == nullptr) {
        std::cerr << "Error getting host by name." << std::endl;
        return -1;
    }

    std::string message = "Hello, World! ------- From Client.";
    std::string end_msg = "\r\n.\r\n";

    // create client socket
    int client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket_fd < 0) {
        std::cerr << "create client socket failed." << std::endl;
        return -1;
    }
    struct sockaddr_in server_sock_addr;
    server_sock_addr.sin_family = AF_INET;
    server_sock_addr.sin_port = htons(PORT);
    std::cout << "create client socket success." << std::endl;
    
    if (host_ptr->h_addr_list[0] == nullptr) {
        std::cerr << "No addresses found for the host." << std::endl;
        close(client_socket_fd);
        return -1;
    }
    memcpy(&server_sock_addr.sin_addr, host_ptr->h_addr_list[0], sizeof(struct in_addr));
    
    // connect to mail server
    if (connect(client_socket_fd, (struct sockaddr*) &server_sock_addr, sizeof(server_sock_addr)) < 0) {
        std::cerr << "connect to mail server failed." << std::endl;
        close(client_socket_fd);
        return -1;
    }

    // receive reply from mail server
    char buffer[SIZE];
    int rec_len = recv(client_socket_fd, buffer, SIZE - 1, 0);
    if (rec_len <= 0) {
        std::cerr << "receive failed." << std::endl;
        close(client_socket_fd);
        return -1;
    }
    buffer[rec_len] = '\0';
    std::cout << buffer << std::endl;

    if (strncmp(buffer, "220", 3) != 0) {
        std::cerr << "220 reply not received from server." << std::endl;
        close(client_socket_fd);
        return -1;
    }

    sendCommand(client_socket_fd, "HELO localhost\r\n", "250");
    sendCommand(client_socket_fd, "MAIL FROM:<540589641@qq.com>\r\n", "250");
    sendCommand(client_socket_fd, "RCPT TO:<chenyinjie666@gmail.com>\r\n", "250");
    sendCommand(client_socket_fd, "DATA\r\n", "354");
    if (send(client_socket_fd, message.c_str(), message.size(), 0) < 0) {
        std::cerr << "send message failed." << std::endl;
        close(client_socket_fd);
        return -1;
    }

    if (send(client_socket_fd, end_msg.c_str(), end_msg.size(), 0) < 0) {
        std::cerr << "send end message failed." << std::endl;
        close(client_socket_fd);
        return -1;
    }
    sendCommand(client_socket_fd, "QUIT\r\n", "221");

    close(client_socket_fd);
    return 0;
}