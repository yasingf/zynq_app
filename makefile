# ָ��������
CC = arm-linux-gnueabihf-gcc

# ����ѡ��
CFLAGS = -Wall -O2 -pthread

# ��Ŀ������Դ�ļ�
SRCS = main.c ddr.c scope.c server.c cJSON.c generater.c switch.c

# �Զ����ɵ�Ŀ���ļ��б��� .o �ļ��ŵ� build �ļ�����
OBJS = $(patsubst %.c,build/%.o,$(SRCS))

# ���ɵĿ�ִ���ļ���
TARGET = server

# Ĭ��Ŀ��
all: build $(TARGET)

# ���� build Ŀ¼
build:
	mkdir -p build

# ����Ŀ���ļ����ɿ�ִ���ļ�
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# ����ÿ��Դ�ļ�ΪĿ���ļ�
build/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# �������ɵ��ļ�
clean:
	rm -rf build $(TARGET)

# ����αĿ��
.PHONY: all clean build
