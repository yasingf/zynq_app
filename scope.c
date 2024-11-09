#include "scope.h"
#include <stdlib.h>  // 添加这个头文件以使用 free
#include "cJSON.h"
#include "server.h" // 确保包含 server_send 的声明


int send_wave() {
    uintptr_t address = 0x30000100; // 要读取的地址
    size_t read_len = 300; // 读取的字节长度，这里假设读取3750字节
    uint8_t buffer[read_len];

    // 从 DDR 中读取数据
    if (ddr_read(address, buffer, read_len) < 0) { // 读取成功时返回的字节数大于或等于0
        printf("Failed to read from DDR\n");
        return -1; // 读取失败
    }

    // 创建 JSON 对象
    cJSON *json_data = cJSON_CreateObject();
    if (json_data == NULL) {
        printf("Failed to create JSON object\n");
        return -1; // 创建 JSON 对象失败
    }

    // 创建一个 JSON 数组来保存波形数据
    cJSON *waveform_array = cJSON_CreateArray();
    if (waveform_array == NULL) {
        printf("Failed to create JSON array\n");
        cJSON_Delete(json_data);
        return -1; // 创建 JSON 数组失败
    }

    // 将读取的数据转换为样本值并添加到 JSON 数组
    for (size_t i = 0; i < read_len; i += 2) {
        if (i + 1 < read_len) { // 确保不会越界
            // 读取 1.5 字节，拼接成一个样本值
            int16_t sample = ((buffer[i] << 4) | (buffer[i + 1] >> 4)) & 0x0FFF; // 仅保留低12位

            // 将无符号数转换为带符号数
            if (sample & 0x0800) { // 如果最高位为1，说明是负数
                sample |= 0xF000; // 扩展符号位
            }

            cJSON_AddItemToArray(waveform_array, cJSON_CreateNumber(sample)); // 将样本值添加到数组
        }
    }

    // 将波形数组添加到 JSON 对象中
    cJSON_AddItemToObject(json_data, "waveform", waveform_array);

    // 发送 JSON 数据
    int result = server_send(json_data); // 直接传递 cJSON 对象

    // 清理 JSON 对象
    cJSON_Delete(json_data); // 清理 JSON 对象

    return result; // 返回发送结果
}

int update_scope(char *parameter,char *data){
    return 0;
}
