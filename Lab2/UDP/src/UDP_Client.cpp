#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <iostream>
#include <string>
#include <cstring>
#include <chrono>

#define PORT 8080
#define SIZE 2048
#define N 10

void send_ping(int client_socket_fd, struct sockaddr_in& server_sock_addr, int sequence_number) {
    auto start_time = std::chrono::system_clock::now();
    std::time_t send_time = std::chrono::system_clock::to_time_t(start_time);
    std::string message = "Ping " + std::to_string(sequence_number) + ": " + std::to_string(send_time);
    if (sendto(client_socket_fd, message.c_str(), message.size(), 0, (const struct sockaddr*) &server_sock_addr, sizeof(server_sock_addr)) < 0) {
        std::cerr << "Ping server failed." << std::endl;
    }
}

bool receive_pong(int client_socket_fd, struct sockaddr_in& server_sock_addr, socklen_t& len, int sequence_number) {
    char buffer[SIZE];
    memset(buffer, 0, SIZE);
    auto start_time = std::chrono::system_clock::now();
    int n = recvfrom(client_socket_fd, buffer, SIZE, 0, (struct sockaddr*) &server_sock_addr, &len);
    auto end_time = std::chrono::system_clock::now();

    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            std::cerr << "Request time out." << std::endl;
        } else {
            perror("Recvfrom error");
        }
        return false;
    } else {
        buffer[n] = '\0';
        std::chrono::duration<double> RTT = end_time - start_time;
        std::cout << "Received from server: " << buffer << std::endl;
        std::cout << "RTT for sequence " << sequence_number << " : " << RTT.count() << "seconds"<< std::endl;
        return true;
    }
}

int main() {
    int client_socket_fd;
    struct sockaddr_in server_sock_addr;
    char buffer[SIZE];

    if ((client_socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Create client socket failed.");
        exit(EXIT_FAILURE);
    }

    std::cout << "Create client socket success." << std::endl;

    memset(&server_sock_addr, 0, sizeof(server_sock_addr));
    server_sock_addr.sin_family = AF_INET;
    server_sock_addr.sin_port = htons(PORT);
    std::string IP = "127.0.0.1";
    if (inet_pton(AF_INET, IP.c_str(), &server_sock_addr.sin_addr) <= 0) {
        std::cerr << "Invalid Address / Address not supported." << std::endl;
        close(client_socket_fd);
        return -1;
    }

    // set Timeout Socket
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    if (setsockopt(client_socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))) {
        perror("setsockopt failed.");
        close(client_socket_fd);
        return -1;
    }

    // Test Ping
    socklen_t addr_len = sizeof(server_sock_addr);
    int timeout_cnt = 0;
    for (int i = 1; i <= N; ++i) {
        std::cout << i << ": ";
        send_ping(client_socket_fd, server_sock_addr, i);
        if (!receive_pong(client_socket_fd, server_sock_addr, addr_len, i)) {
            ++timeout_cnt;
        }
        std::cout << "-------------------------------\n";
    }

    if (timeout_cnt > N / 2) {
        std::cerr << "Connection unstable. Timeout count: " << timeout_cnt << std::endl;
        close(client_socket_fd);
        return -1;
    }

    std::cout << "start communication with server: " << std::endl;
    std::string msg;
    while (true) {
        std::cout << "Enter message to send (or 'exit' to quit): ";
        std::getline(std::cin, msg);
        if (msg == "exit") break;
        if (sendto(client_socket_fd, msg.c_str(), msg.size(), 0, (const struct sockaddr*) &server_sock_addr, sizeof(server_sock_addr)) < 0) {
            std::cerr << "Failed to send message." << std::endl;
            continue;
        }
        int n = recvfrom(client_socket_fd, buffer, SIZE, 0, (struct sockaddr*) &server_sock_addr, &addr_len);
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                std::cerr << "Response timed out." << std::endl;
            } else {
                perror("recvfrom error");
            }
        } else {
            buffer[n] = '\0';
            std::cout << "Server response: " << buffer << std::endl;
        }
    }
    close(client_socket_fd);

    return 0;
}