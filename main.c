#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "ddr.h"
#include "server.h"
#include "switch.h"
#include "scope.h"
#include "cJSON.h"
#include "generater.h"

#define BUFFER_SIZE 1024

void *send_wave_thread(void *arg); // 声明发送波形线程
void process_data(cJSON *json);    // 处理 JSON 数据
int ins = 0;

int main()
{
    ddr_init(0x30000000, 268435456); // 初始化 DDR

    if (server_init(SERVER_IP, SERVER_PORT) < 0)
    {
        fprintf(stderr, "Failed to initialize server\n");
        exit(EXIT_FAILURE);
    }

    pthread_t wave_thread; // 声明线程
    while (1)
    {
        // 接收 JSON 数据
        cJSON *json = server_listen();
        if (json)
        {
            process_data(json); // 处理 JSON 数据
            cJSON_Delete(json); // 清理 JSON 对象
        }

        // 在需要发送波形时启动线程
        if (ins == 1)
        {
            if (pthread_create(&wave_thread, NULL, send_wave_thread, NULL) != 0)
            {
                perror("Failed to create thread");
                exit(EXIT_FAILURE);
            }
            pthread_detach(wave_thread); // 分离线程，确保资源自动回收
        }
        else if (ins == 2)
        {
            printf("信号发生器\n");
        }
    }

    server_close(); // 关闭服务器
    return 0;
}

void *send_wave_thread(void *arg)
{
    // 发送波形的函数
    while (1)
    {
        send_wave();
    }

    // 执行发送波形的功能
    return NULL; // 返回 NULL
}

void process_data(cJSON *json)
{
    // 从 JSON 对象中获取 cmd_type
    cJSON *cmd_type_item = cJSON_GetObjectItem(json, "cmd_type");
    if (cmd_type_item == NULL || !cJSON_IsString(cmd_type_item))
    {
        printf("Invalid or missing cmd_type\n");
        return;
    }

    const char *cmd_type = cmd_type_item->valuestring; // 获取 cmd_type 字符串

    if (strcmp(cmd_type, "switch") == 0)
    {
        printf("Switching instrument...\n");
        cJSON *ins_item = cJSON_GetObjectItem(json, "instrument");
        if (ins_item && cJSON_IsString(ins_item))
        {
            ins_switch(ins_item->valuestring); // 切换仪器
            ins = (strcmp(ins_item->valuestring, "scope") == 0) ? 1 : (strcmp(ins_item->valuestring, "generator") == 0) ? 2
                                                                                                                        : 0;
        }
    }
    else if (strcmp(cmd_type, "update") == 0)
    {
        cJSON *parameter_item = cJSON_GetObjectItem(json, "parameter");
        cJSON *data_item = cJSON_GetObjectItem(json, "data");
        char *parameter = parameter_item->valuestring;
        char *data = data_item->valuestring;
        if (ins == 2)
        {
            update_generater(parameter, data);
        }
        else if (ins == 1)
        {
            update_scope(parameter, data);
        }
    }
    else if (strcmp(cmd_type, "exitins") == 0)
    {
        printf("返回默认");
        ins = 0;
    }
    else if (strcmp(cmd_type, "custom") == 0)
    {
        printf("接受自定义仪器中...\n");
        cJSON *custom_item = cJSON_GetObjectItem(json, "custom_instrument");
        if (custom_item && cJSON_IsNumber(custom_item))
        {
            custom_switch((uint8_t)custom_item->valueint); // 切换自定义仪器
        }
    }
    else
    {
        printf("Unknown command type: %s\n", cmd_type);
    }
}
