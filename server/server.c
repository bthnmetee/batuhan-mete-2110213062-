// Güncellenmiş Server Kodu:
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")  // Windows Sockets Library

#define MAX 512
#define SA struct sockaddr

void fileTransfer(SOCKET sockfd) {
    char messageBuffer[MAX];
    char fileName[MAX];
    char fullFileName[MAX] = "C:\\Users\\Global\\Desktop\\Alperem\\server";

    while (1) {
        memset(messageBuffer, 0, MAX);

        // Server: Read file name request from CLIENT
        int bytesReceived = recv(sockfd, messageBuffer, MAX, 0);
        if (bytesReceived <= 0) break;

        printf("CLIENT: Requesting %s \n", messageBuffer);

        if (strncmp(messageBuffer, "exit", 4) == 0) {
            send(sockfd, messageBuffer, strlen(messageBuffer), 0);
            printf("SERVER: Exiting...\n");
            break;
        }

        strcpy(fileName, messageBuffer);
        strcat(fullFileName, fileName);
        FILE* serverFile = fopen(fullFileName, "rb");

        if (serverFile != NULL) {
            send(sockfd, "OK", 2, 0);
            memset(messageBuffer, 0, MAX);
            recv(sockfd, messageBuffer, MAX, 0);

            if (strncmp(messageBuffer, "OK", 2) == 0) {
                int blockSize;
                memset(messageBuffer, 0, MAX);
                printf("Server: Sending file %s to Client... \n", fileName);

                while ((blockSize = fread(messageBuffer, 1, MAX, serverFile)) > 0) {
                    send(sockfd, messageBuffer, blockSize, 0);
                    memset(messageBuffer, 0, MAX);
                }
                printf("SERVER: File successfully sent to client! \n");
            }
            fclose(serverFile);
        } else {
            printf("SERVER: ERROR file %s not found on server. \n", fullFileName);
            send(sockfd, "NULL", 4, 0);
        }
    }
    closesocket(sockfd);
}

int main(int argc, char* argv[]) {
    WSADATA wsaData;
    char* port;
    int portVal;
    SOCKET serverSocket;
    int clientLength;
    SOCKET connection;
    struct sockaddr_in serverAddress;
    struct sockaddr_in client;

    if (argc < 2) {
        port = "8080";
    } else {
        port = argv[1];
    }

    portVal = atoi(port);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("SERVER: Winsock initialization failed. \n");
        exit(0);
    } else {
        printf("SERVER: Winsock initialization successful. \n");
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("SERVER: Socket creation failed. \n");
        WSACleanup();
        exit(0);
    } else {
        printf("SERVER: Socket creation successful. \n");
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(portVal);

    if ((bind(serverSocket, (SA*)&serverAddress, sizeof(serverAddress))) != 0) {
        printf("SERVER: Socket binding failed. \n");
        closesocket(serverSocket);
        WSACleanup();
        exit(0);
    } else {
        printf("SERVER: Socket binding successful. \n");
    }

    if ((listen(serverSocket, 5)) != 0) {
        printf("SERVER: Listening on port %d failed. \n", portVal);
        closesocket(serverSocket);
        WSACleanup();
        exit(0);
    } else {
        printf("SERVER: Listening on Port %d... \n", portVal);
    }

    clientLength = sizeof(client);
    connection = accept(serverSocket, (SA*)&client, &clientLength);
    if (connection == INVALID_SOCKET) {
        printf("SERVER: Client acceptance failed. \n");
        closesocket(serverSocket);
        WSACleanup();
        exit(0);
    } else {
        printf("SERVER: Client acceptance successful. \n");
    }

    fileTransfer(connection);

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
