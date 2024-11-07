# 指定编译器
CC = arm-linux-gnueabihf-gcc

# 编译选项
CFLAGS = -Wall -O2 -pthread

# 项目中所有源文件
SRCS = main.c ddr.c scope.c server.c cJSON.c generater.c switch.c

# 自动生成的目标文件列表，将 .o 文件放到 build 文件夹中
OBJS = $(patsubst %.c,build/%.o,$(SRCS))

# 生成的可执行文件名
TARGET = server

# 默认目标
all: build $(TARGET)

# 创建 build 目录
build:
	mkdir -p build

# 链接目标文件生成可执行文件
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# 编译每个源文件为目标文件
build/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清理生成的文件
clean:
	rm -rf build $(TARGET)

# 声明伪目标
.PHONY: all clean build
