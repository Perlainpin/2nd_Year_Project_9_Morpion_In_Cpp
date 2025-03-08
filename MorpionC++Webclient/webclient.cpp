#pragma comment(lib, "ws2_32.lib")

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#define PORT 80

int main(int argc, char const* argv[])
{
    int status, valread, client_fd;
    struct addrinfo hints, * result, * p;
    char buffer[1024] = { 0 };

    // Initialize Winsock
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock\n");
        return -1;
    }

    // Setup hints for getaddrinfo
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // Use IPv4
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    if ((status = getaddrinfo("127.0.0.1", "http", &hints, &result)) != 0) {
        printf("getaddrinfo failed: %d\n", status);
        WSACleanup();
        return -1;
    }

    // Iterate through the results and connect
    for (p = result; p != NULL; p = p->ai_next) {
        if ((client_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == INVALID_SOCKET) {
            printf("\n Socket creation error \n");
            continue;
        }

        if ((status = connect(client_fd, p->ai_addr, p->ai_addrlen)) == SOCKET_ERROR) {
            closesocket(client_fd);
            printf("\nConnection Failed \n");
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (p == NULL) {
        printf("Failed to connect\n");
        WSACleanup();
        return -1;
    }

    const char* http_request = "GET / HTTP/1.1\r\nHost: 127.0.0.1\r\nConnection: close\r\n\r\n";
    send(client_fd, http_request, strlen(http_request), 0);
    printf("HTTP request sent\n");

    // Receive and print the response
    do {
        valread = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (valread > 0) {
            buffer[valread] = '\0';  // null-terminate the received data
            printf("%s", buffer);
        }
    } while (valread > 0);

    // closing the connected socket
    closesocket(client_fd);
    WSACleanup();
    return 0;
}