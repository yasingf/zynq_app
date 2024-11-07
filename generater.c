/*
0x3000_0000
��(1byte/4��
0����
1����
2���
3����
)
Ƶ��(2byte)
��һbyte(��λhz,khz,mhz)
�ڶ�byte(Ƶ��0��1000)
����(1byte, 1��2��4��8��16)
 */

#include "generater.h"

int update_generater(char *parameter, char *data)
{
    if (strcmp(parameter, "wave") == 0)
    {
        uintptr_t address = 0x30000000;
        ddr_write(address, data, 1);
    }
    else if (strcmp(parameter, "frequency") == 0)
    {
        uintptr_t address = 0x30000001;
        ddr_write(address, data, 2);
    }
    else if (strcmp(parameter, "amplitude") == 0)
    {
        uintptr_t address = 0x30000003;
        ddr_write(address, data, 1);
    }
    return 0;
}