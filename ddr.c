#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define DDR_BASE_ADDR  0x30000000    // DDR 基地址
#define DDR_SIZE       0x10000000    // DDR 大小（256MB）

static void *ddr_virt_base = NULL;   // DDR 虚拟基地址

/**
 * 初始化DDR映射
 * @return 成功返回0，失败返回-1
 */
int ddr_init() {
    int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd == -1) {
        perror("无法打开 /dev/mem");
        return -1;
    }

    // 将DDR的物理地址空间映射到进程虚拟地址空间
    ddr_virt_base = mmap(NULL, DDR_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, DDR_BASE_ADDR);
    close(mem_fd);

    if (ddr_virt_base == MAP_FAILED) {
        perror("mmap 失败");
        ddr_virt_base = NULL;
        return -1;
    }

    return 0;
}

/**
 * 从DDR特定地址读取数据
 * @param address DDR的物理地址
 * @param buffer 存储读取数据的缓冲区
 * @param length 要读取的字节数
 * @return 成功返回读取的字节数，失败返回-1
 */
ssize_t ddr_read(uintptr_t address, void *buffer, size_t length) {
    if (!ddr_virt_base) {
        fprintf(stderr, "DDR 未初始化\n");
        return -1;
    }

    // 确保地址在映射范围内
    if (address < DDR_BASE_ADDR || address + length > DDR_BASE_ADDR + DDR_SIZE) {
        fprintf(stderr, "读取地址超出DDR范围\n");
        return -1;
    }

    // 计算读取起始地址在映射内的虚拟地址
    void *source = (uint8_t *)ddr_virt_base + (address - DDR_BASE_ADDR);
    memcpy(buffer, source, length);
    return length;
}

/**
 * 向DDR特定地址写入数据
 * @param address DDR的物理地址
 * @param buffer 包含要写入数据的缓冲区
 * @param length 要写入的字节数
 * @return 成功返回写入的字节数，失败返回-1
 */
ssize_t ddr_write(uintptr_t address, const void *buffer, size_t length) {
    if (!ddr_virt_base) {
        fprintf(stderr, "DDR 未初始化\n");
        return -1;
    }

    // 确保地址在映射范围内
    if (address < DDR_BASE_ADDR || address + length > DDR_BASE_ADDR + DDR_SIZE) {
        fprintf(stderr, "写入地址超出DDR范围\n");
        return -1;
    }

    // 计算写入起始地址在映射内的虚拟地址
    void *dest = (uint8_t *)ddr_virt_base + (address - DDR_BASE_ADDR);
    memcpy(dest, buffer, length);
    return length;
}

/**
 * 解除DDR映射
 */
void ddr_cleanup() {
    if (ddr_virt_base) {
        munmap(ddr_virt_base, DDR_SIZE);
        ddr_virt_base = NULL;
    }
}