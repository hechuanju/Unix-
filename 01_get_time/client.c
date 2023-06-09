
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 8888

int main(int argc, char const *argv[])
{
    int socket_fd = 0;
    struct sockaddr_in server_addr;
    char buffer[1024];

    // 创建套接字
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("Error in socket");
        exit(EXIT_FAILURE);
    }

    // 设置服务器地址
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_ADDR, &(server_addr.sin_addr)) < 0) { // 转换网路地址
        perror("Error in address");
        exit(EXIT_FAILURE);
    }

    // 连接服务器
    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error in connect");
        exit(EXIT_FAILURE);
    }

    // 读取时间数据
    memset(buffer, '\0', sizeof(buffer));
    if (recv(socket_fd, buffer, sizeof(buffer), 0) < 0) {
        perror("Error in receiving");
        exit(EXIT_FAILURE);
    }

    printf("server time: %s\n", buffer);

    close(socket_fd);
    
    return 0;
}
