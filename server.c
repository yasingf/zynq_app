#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <error.h>
#include "cJSON.h"

static int server_socket = -1;
static int client_socket = -1;

/**
 * @brief 服务器初始化
 * @param ip 服务器IP地址
 * @param port 端口号
 * @return int 成功返回0，失败返回-1
 */
int server_init(const char *ip, int port)
{
    struct sockaddr_in server_addr;

    // 创建TCP套接字
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket creation failed");
        return -1;
    }

    // 设置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    // 绑定地址和端口
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        close(server_socket);
        return -1;
    }

    // 设置监听队列大小
    if (listen(server_socket, 5) < 0)
    {
        perror("Listen failed");
        close(server_socket);
        return -1;
    }

    printf("Server listening on: %s:%d\n", ip, port);
    return 0;
}

void wait_for_connection()
{
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // 等待客户端连接
    printf("Waiting for client connection...\n");
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
    if (client_socket < 0)
    {
        perror("Accept failed");
        return;
    }

    printf("Client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
}

// 接收数据并返回解析后的 JSON 对象
cJSON *receive_data()
{
    char buffer[1024] = {0};

    // 接收数据
    ssize_t recv_len = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (recv_len < 0)
    {
        perror("Receive failed");
        return NULL;
    }

    buffer[recv_len] = '\0'; // 确保字符串以空字符结束

    // 解析 JSON 数据
    cJSON *json = cJSON_Parse(buffer);
    if (json == NULL)
    {
        printf("Failed to parse JSON data\n");
        return NULL;
    }

    return json; // 返回解析得到的 JSON 对象
}

/**
 * @brief 向上位机发送 JSON 格式的数据，带包头
 * @param json_data JSON 对象
 * @return int 成功返回0，失败返回-1
 */
int server_send(cJSON *json_data)
{
    if (client_socket < 0)
    {
        printf("No client connected\n");
        return -1;
    }

    // 将 JSON 对象转换为字符串
    char *json_str = cJSON_PrintUnformatted(json_data);
    if (json_str == NULL)
    {
        printf("Failed to print JSON data\n");
        return -1;
    }

    // 获取数据长度并生成包头
    uint32_t data_len = strlen(json_str);
    uint32_t packet_len = htonl(data_len);  // 将长度转为网络字节序

    // 发送包头（数据长度）
    ssize_t sent_len = send(client_socket, &packet_len, sizeof(packet_len), 0);
    if (sent_len != sizeof(packet_len))
    {
        perror("Send header failed");
        free(json_str);
        return -1;
    }

    // 发送实际数据
    sent_len = send(client_socket, json_str, data_len, 0);
    if (sent_len != data_len)
    {
        perror("Send data failed");
        free(json_str);
        return -1;
    }

    // printf("Sent JSON data with header: %s\n", json_str);

    // 清理
    free(json_str);
    return 0;
}

/**
 * @brief 关闭服务器
 */
void server_close()
{
    if (client_socket >= 0)
    {
        close(client_socket);
        client_socket = -1;
    }
    if (server_socket >= 0)
    {
        close(server_socket);
        server_socket = -1;
    }
    printf("Server closed\n");
}

