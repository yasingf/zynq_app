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
void stop_wave_thread();           // 停止波形线程的函数

int ins = 0;
pthread_t wave_thread;
pthread_mutex_t ins_mutex = PTHREAD_MUTEX_INITIALIZER;
int wave_thread_running = 0; // 表示波形线程是否正在运行的标志
int wave_thread_exit = 0;    // 表示波形线程是否应退出
int json_updated = 0;        // 新的 JSON 数据标志

int main()
{
    ddr_init(0x30000000, 268435456); // 初始化 DDR

    // 初始连接服务器
    if (server_init(SERVER_IP, SERVER_PORT) < 0)
    {
        fprintf(stderr, "Failed to initialize server\n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        wait_for_connection();
        while (1)
        {
            cJSON *json = receive_data();
            if (json != NULL)
            {
                process_data(json); // 处理 JSON 数据
                json_updated = 1;   // 标记 JSON 数据已更新
                cJSON_Delete(json); // 清理 JSON 对象
            }
            else
            {
                break;
            }
            if (json_updated)
            {
                // 检查是否需要启动波形发送线程
                pthread_mutex_lock(&ins_mutex);
                if (ins == 1 && !wave_thread_running)
                {
                    wave_thread_running = 1;
                    wave_thread_exit = 0;
                    if (pthread_create(&wave_thread, NULL, send_wave_thread, NULL) != 0)
                    {
                        perror("Failed to create wave thread");
                        wave_thread_running = 0;
                    }
                    pthread_detach(wave_thread); // 分离线程，确保资源自动回收
                }
                pthread_mutex_unlock(&ins_mutex);

                // 处理完后重置 json_updated，防止再次进入第二个 while 循环
                json_updated = 0;
            }
        }
    }

    server_close(); // 关闭服务器
    return 0;
}

void *send_wave_thread(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&ins_mutex);
        if (wave_thread_exit)
        {
            wave_thread_running = 0;
            pthread_mutex_unlock(&ins_mutex);
            printf("Wave thread is exiting...\n"); // 输出线程退出日志
            break;                                 // 退出线程
        }
        pthread_mutex_unlock(&ins_mutex);

        send_wave(); // 执行发送波形的功能
    }

    return NULL;
}

void stop_wave_thread()
{
    pthread_mutex_lock(&ins_mutex);
    if (wave_thread_running)
    {
        wave_thread_exit = 1;                // 设置退出标志
        printf("Stopping wave thread...\n"); // 输出停止线程日志
        pthread_mutex_unlock(&ins_mutex);

        // 等待线程退出
        while (wave_thread_running)
        {
            usleep(10000); // 等待线程退出
        }
    }
    else
    {
        pthread_mutex_unlock(&ins_mutex);
    }

    ins = 0; // 重置仪器状态
    printf("Wave thread stopped, returning to default state.\n");
}

void process_data(cJSON *json)
{
    cJSON *cmd_type_item = cJSON_GetObjectItem(json, "cmd_type");
    if (cmd_type_item == NULL || !cJSON_IsString(cmd_type_item))
    {
        printf("Invalid or missing cmd_type\n");
        return;
    }

    const char *cmd_type = cmd_type_item->valuestring;

    if (strcmp(cmd_type, "switch") == 0)
    {
        printf("Switching instrument...\n");
        cJSON *ins_item = cJSON_GetObjectItem(json, "instrument");
        if (ins_item && cJSON_IsString(ins_item))
        {
            ins_switch(ins_item->valuestring); // 切换仪器
            pthread_mutex_lock(&ins_mutex);
            ins = (strcmp(ins_item->valuestring, "scope") == 0) ? 1 : (strcmp(ins_item->valuestring, "generator") == 0) ? 2
                                                                                                                        : 0;
            pthread_mutex_unlock(&ins_mutex);
        }
    }
    else if (strcmp(cmd_type, "update") == 0)
    {
        pthread_mutex_lock(&ins_mutex);
        if (ins == 2)
        {
            cJSON *waveform_item = cJSON_GetObjectItem(json, "waveform");
            cJSON *frequency_item = cJSON_GetObjectItem(json, "frequency");
            cJSON *amplitude_item = cJSON_GetObjectItem(json, "amplitude");

            int waveform = waveform_item ? waveform_item->valueint : -1;
            int frequency = frequency_item ? frequency_item->valueint : -1;
            int amplitude = amplitude_item ? amplitude_item->valueint : -1;
            update_generater(waveform, frequency, amplitude);
        }
        else if (ins == 1)
        {
            // update_scope(parameter, data);
        }
        pthread_mutex_unlock(&ins_mutex);
    }
    else if (strcmp(cmd_type, "exitins") == 0)
    {
        printf("Returning to default state...\n");
        stop_wave_thread(); // 停止波形线程
    }
    else if (strcmp(cmd_type, "custom") == 0)
    {
        printf("Switching to custom instrument...\n");
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
