#!/bin/bash






# 检查系统类型
if [ -f /etc/centos-release ]; then
    # CentOS 系统
    echo "检测到 CentOS 系统，开始安装..."
    sudo yum update -y
    sudo yum install -y gcc gcc-c++ cmake gdb valgrind libstdc++-dev 
    echo "memcheck 是 Valgrind 的一部分，已安装 Valgrind"
elif [ -f /etc/lsb-release ]; then
    # Ubuntu 系统
    echo "检测到 Ubuntu 系统，开始安装..."
    sudo apt update -y
    sudo apt install -y build-essential cmake gdb valgrind
    sudo apt install -y libstdc++-dev
    echo "memcheck 是 Valgrind 的一部分，已安装 Valgrind"
else
    echo "无法识别的 Linux 发行版"
    exit 1
fi

echo "所有请求的软件包已成功安装。"
