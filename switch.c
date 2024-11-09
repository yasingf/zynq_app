#include "switch.h"
#include <string.h>

void ins_switch(char *ins)
{
    system("echo 0 > /sys/class/fpga_manager/fpga0/flags");
    system("mkdir -p /lib/firmware");
    if(strcmp(ins,"scope") == 0){
        printf("switch to Scope\n");
        system("cp /home/xilinx/server/bin/scope.bin /lib/firmware");
        system("echo scope.bin > /sys/class/fpga_manager/fpga0/firmware");
    }else if(strcmp(ins,"generator") == 0){
        printf("switch to Generater\n");
        system("cp /home/xilinx/server/bin/generator.bin /lib/firmware");
        system("echo generator.bin > /sys/class/fpga_manager/fpga0/firmware");
    }else{
        printf("Illegal cmd\n");
    }
}
void custom_switch(uint8_t instrument)
{
    char command1[100];
    char command2[100];
    system("echo 0 > /sys/class/fpga_manager/fpga0/flags");
    system("mkdir -p /lib/firmware");
    sprintf(command1,"cp /home/xilinx/server/bin/%hhu.bin /lib/firmware",instrument);
    sprintf(command2,"echo %d.bin > /sys/class/fpga_manager/fpga0/firmware",instrument);
    system(command1);
    system(command2);
}