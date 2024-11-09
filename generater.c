/*
0x3000_0000
波(1byte/4种
0正弦
1三角
2锯齿
3方波
)
频率(2byte)
第一byte(单位hz,khz,mhz)
第二byte(频率0～1000)
幅度(1byte, 1，2，4，8，16)
 */

#include "generater.h"
#include <stdint.h>  // 确保使用标准整数类型

int update_generater(int waveform, int frequency, int amplitude)
{
    // 处理 waveform，将其转换为无符号整数并写入指定地址
    if (waveform >= 0)
    {
        uintptr_t address = 0x30000000;
        uint8_t waveform_data = (uint8_t)waveform;  // 转换为 8 位无符号数
        ddr_write(address, &waveform_data, sizeof(waveform_data));
    }

    // 处理 frequency，将其转换为无符号整数并写入指定地址
    if (frequency >= 0)
    {
        uintptr_t address = 0x30000002;
        uint32_t frequency_data = (uint32_t)(((uint64_t)frequency * (1ULL << 32)) / 125000000);
        ddr_write(address, &frequency_data, sizeof(frequency_data));
    }

    // 处理 amplitude，将其转换为无符号整数并写入指定地址
    if (amplitude >= 0)
    {
        uintptr_t address = 0x30000001;
        uint8_t amplitude_data = (uint8_t)amplitude;  // 转换为 8 位无符号数
        ddr_write(address, &amplitude_data, sizeof(amplitude_data));
    }

    return 0;
}
