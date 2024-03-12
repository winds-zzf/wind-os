########################
#   Makefile脚本生成    #
########################

# 编译器
CC := gcc

# 编译选项
CFLAGS := -Wall -Wextra

# 链接选项
LDFLAGS :=

# 源文件目录
SRC_DIR := ./

# 源文件列表
SRCS := $(wildcard $(SRC_DIR)*.c)

# 目标文件列表
OBJS := $(SRCS:.c=.o)

# 可执行文件
TARGET := scripter

# 默认目标
all: clean $(TARGET)

# 生成目标文件
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 生成可执行文件
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

# 清理生成的文件
clean:
	rm -f $(OBJS) $(TARGET)
