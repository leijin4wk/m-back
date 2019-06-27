cmake_minimum_required(VERSION 3.10)
project(m_back C)

set(CMAKE_C_STANDARD 99)

##
# 查找目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(src/ DIR_SRCS)

add_executable(m_back ${DIR_SRCS})