#ifndef DDR_H
#define DDR_H
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
// DDR基地址和大小
#define DDR_BASE_ADDR  0x30000000
#define DDR_SIZE       0x10000000  // 256MB

/**
 * 初始化DDR映射
 * @return 成功返回0，失败返回-1
 * 
 * 该函数会打开 /dev/mem，并将DDR的物理地址空间映射到虚拟地址空间中。
 * 初始化后可以通过 ddr_read 和 ddr_write 函数进行数据操作。
 */
int ddr_init();

/**
 * 从DDR特定地址读取数据
 * @param address 要读取的DDR物理地址
 * @param buffer 用于存储读取数据的缓冲区
 * @param length 要读取的字节数
 * @return 成功返回读取的字节数，失败返回-1
 * 
 * 从DDR中的指定物理地址读取数据，并将其存储到提供的缓冲区中。
 */
ssize_t ddr_read(uintptr_t address, void *buffer, size_t length);

/**
 * 向DDR特定地址写入数据
 * @param address 要写入的DDR物理地址
 * @param buffer 包含要写入数据的缓冲区
 * @param length 要写入的字节数
 * @return 成功返回写入的字节数，失败返回-1
 * 
 * 向DDR中的指定物理地址写入数据。
 */
ssize_t ddr_write(uintptr_t address, const void *buffer, size_t length);

/**
 * 解除DDR映射
 * 
 * 该函数解除对DDR的虚拟地址映射，释放映射资源。使用完DDR后应调用此函数。
 */
void ddr_cleanup();

#endif // DDR_H