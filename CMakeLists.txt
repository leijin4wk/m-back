cmake_minimum_required(VERSION 3.10)
project(m_back C)

set(CMAKE_C_STANDARD 11)


#条件编译
option (USE_A  "Use A" ON)
message("USE_A option is ${USE_A}")
if (USE_A)
    message("aaa")
    add_definitions(-DUSE_A)
endif (USE_A)

option (USE_B  "Use B"  OFF)
message("USE_B option is ${USE_B}")
if (USE_B)
    message("bbb")
    add_definitions(-DUSE_B)
endif (USE_B)

##
# 查找目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(src/ DIR_SRCS)

add_executable(m_back ${DIR_SRCS} src/socket.h src/socket.c)