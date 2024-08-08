#include <iostream>
#include <cstring>
#include <string>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define PORT 587
#define SIZE 1024

void sendCommand(SSL* ssl, const std::string& command, const std::string& response) {
    char buffer[SIZE];
    
}

int main() {
    // Initalize SSL
    // Load all the error msg
    SSL_load_error_strings();
    // load all the secure algorithmn
    OpenSSL_add_all_algorithms();
    

    
    return 0;
}

