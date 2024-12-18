// Güncellenmiş Client Kodu:
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
    char fullFileName[MAX] = "testfiles/";

    while (1) {
        memset(messageBuffer, 0, MAX);

        printf("CLIENT: Enter the filename you wish to download, or type 'exit' to close connection: ");
        scanf("%s", messageBuffer);
        send(sockfd, messageBuffer, strlen(messageBuffer), 0);

        if (strncmp(messageBuffer, "exit", 4) == 0) {
            memset(messageBuffer, 0, MAX);
            recv(sockfd, messageBuffer, MAX, 0);
            printf("CLIENT: Exiting...\n");
            break;
        }

        strcpy(fileName, messageBuffer);
        strcat(fullFileName, fileName);
        memset(messageBuffer, 0, MAX);
        recv(sockfd, messageBuffer, MAX, 0);

        if (strncmp(messageBuffer, "OK", 2) == 0) {
            send(sockfd, "OK", 2, 0);
            FILE *clientFile = fopen(fileName, "wb");

            if (clientFile == NULL) {
                printf("CLIENT: ERROR File %s cannot be opened. \n", fileName);
                break;
            } else {
                int blockSize;
                printf("CLIENT: Receiving %s from Server and saving it. \n", fileName);

                while ((blockSize = recv(sockfd, messageBuffer, MAX, 0)) > 0) {
                    fwrite(messageBuffer, sizeof(char), blockSize, clientFile);
                    memset(messageBuffer, 0, MAX);
                }

                printf("CLIENT: File received from server! \n");
                fclose(clientFile);
            }
        } else {
            printf("SERVER: File %s not found on server, please try another file. \n", fileName);
        }
    }
    closesocket(sockfd);
}

int main(int argc, char* argv[]) {
    WSADATA wsaData;
    char* ipAddress;
    char* port;
    int portVal;
    SOCKET clientSocket;
    struct sockaddr_in serverAddress;

    if (argc < 2) {
        ipAddress = "127.0.0.1";
        port = "8080";
    } else if (argc < 3) {
        ipAddress = argv[1];
        port = "8080";
    } else {
        ipAddress = argv[1];
        port = argv[2];
    }

    portVal = atoi(port);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("CLIENT: Winsock initialization failed. \n");
        exit(0);
    } else {
        printf("CLIENT: Winsock initialization successful. \n");
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        printf("CLIENT: Socket creation failed. \n");
        WSACleanup();
        exit(0);
    } else {
        printf("CLIENT: Socket creation successful. \n");
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(ipAddress);
    serverAddress.sin_port = htons(portVal);

    if (connect(clientSocket, (SA*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        printf("CLIENT: Connection with the server failed. \n");
        closesocket(clientSocket);
        WSACleanup();
        exit(0);
    } else {
        printf("CLIENT: Connected to %s:%d \n", ipAddress, portVal);
    }

    fileTransfer(clientSocket);

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
