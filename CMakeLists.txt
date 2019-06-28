cmake_minimum_required(VERSION 3.10)
project(m_back C)

set(CMAKE_C_STANDARD 11)


#条件编译
option (USE_A  "Use A" ON)
if (USE_A)
    message("add USE_A def")
    add_definitions(-DUSE_A)
endif (USE_A)

option (USE_B  "Use B"  OFF)
if (USE_B)
    message("add USE_B def")
    add_definitions(-DUSE_B)
endif (USE_B)

##
# 查找目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(src/ DIR_SRCS)
add_executable(m_back ${DIR_SRCS})
target_link_libraries(m_back pthread)