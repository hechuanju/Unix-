#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <unistd.h>

#define SERVER_PORT 8888
#define BUFFER_SIZE 1024

void handleClient(int clientSocket) {
    char buffer[BUFFER_SIZE];
    time_t currentTime;
    struct tm* timeInfo;
    char timeString[BUFFER_SIZE];

    // 获取当前时间
    currentTime = time(NULL);
    timeInfo = localtime(&currentTime);
    strftime(timeString, BUFFER_SIZE, "%Y-%m-%d %H:%M:%S", timeInfo);

    // 发送时间给客户端
    if (send(clientSocket, timeString, strlen(timeString), 0) < 0) {
        perror("Error in sending");
        exit(EXIT_FAILURE);
    }

    printf("Time sent to client: %s\n", timeString);

    // 关闭客户端套接字
    close(clientSocket);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    pid_t childPid;

    // 创建套接字
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Error in socket");
        exit(EXIT_FAILURE);
    }

    // 设置服务器地址
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);

    // 绑定套接字到指定地址和端口
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Error in binding");
        exit(EXIT_FAILURE);
    }

    // 监听连接请求
    if (listen(serverSocket, 1) < 0) {
        perror("Error in listening");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", SERVER_PORT);

    while (1) {
        // 接受客户端连接请求
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
        if (clientSocket < 0) {
            perror("Error in accepting");
            exit(EXIT_FAILURE);
        }

        // 创建子进程处理客户端请求
        childPid = fork();
        if (childPid < 0) {
            perror("Error in forking");
            exit(EXIT_FAILURE);
        } else if (childPid == 0) {
            // 子进程
            close(serverSocket); // 在子进程中关闭“监听套接字”，监听工作依旧由父进程来完成
            handleClient(clientSocket);
            exit(EXIT_SUCCESS);
        } else {
            // 父进程
            close(clientSocket); // 新的客户请求已经有子进程提供服务，所以父进程的可以关闭了（注意，socket描述符是由引用计数的）
        }
    }

    // 关闭服务器套接字
    close(serverSocket);

    return 0;
}
