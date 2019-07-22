cmake_minimum_required(VERSION 3.5)
project(m_back C)

set(CMAKE_C_STANDARD 99)

MESSAGE(STATUS "Current Operation System: ${CMAKE_SYSTEM}")

MESSAGE(STATUS "Current Operation System Name: ${CMAKE_SYSTEM_NAME}")

MESSAGE(STATUS "Current Operation System Name: ${CMAKE_SYSTEM_PROCESSOR}")

add_definitions(-D_GNU_SOURCE)
#条件编译
option (DEBUG  "DEBUG" ON)
if (DEBUG)
    message("DEBUG OPEN")
    add_definitions(-DDEBUG)
endif ()
##包含zdb头文件
include_directories(/usr/include/zdb)
##包含libjson-c头文件
include_directories(/usr/local/include/cjson)
##包含自己的头文件
include_directories(./include)
# 查找目录下的所有源文件，并将名称保存到 DIR_SRCS 变量
aux_source_directory(src/ DIR_SRCS)
##home 动态库
add_subdirectory(web/home)
##route 动态库
add_subdirectory(web/route)
##生成可执行代码
add_executable(m_back ${DIR_SRCS} web/route/route.c)

target_link_libraries(m_back /usr/local/lib/libcjson.so)
#pthread  posix的线程库
target_link_libraries(m_back pthread)
#zdb  数据库连接池库
target_link_libraries(m_back zdb)
#http_parser  http解析库
target_link_libraries(m_back http_parser)
#https  stl openssl依赖库
target_link_libraries(m_back ssl crypto)
#json 解析库
target_link_libraries(m_back cjson)
#dl
target_link_libraries(m_back dl)
