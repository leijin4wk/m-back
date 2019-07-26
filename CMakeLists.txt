cmake_minimum_required(VERSION 3.5)
project(m_back C)

set(CMAKE_C_STANDARD 99)

MESSAGE(STATUS "Current Operation System Name:${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_PROCESSOR}")
##显示编译细节信息
##set(CMAKE_VERBOSE_MAKEFILE ON)

#条件编译
option (DEBUG  "DEBUG" ON)
if (DEBUG)
    message("DEBUG OPEN")
    add_definitions(-DDEBUG)
endif()

##添加链接器参数
##使用dlopen去动态加载so, 那么链接时ld并不知道可执行文件中的函数会被外部引用, 也就不会导出函数到动态符号表去.
##当dlopen打开so时, 动态链接器在全局符号表中找不到函数符号, 就报错了，见 程序员的自我修养中第7.6.2章
set(CMAKE_C_FLAGS  "-Wl,-E")
MESSAGE(STATUS "${CMAKE_C_FLAGS}")

##包含zdb头文件
include_directories(/usr/include/zdb)
##包含自己的头文件
include_directories(./include)
# 查找目录下的所有源文件，并将名称保存到 DIR_SRCS 变量
aux_source_directory(src/ DIR_SRCS)
##home 动态库
add_subdirectory(web/home)
##route 动态库
add_subdirectory(web/route)
##生成可执行代码
add_executable(m_back ${DIR_SRCS})
#pthread  posix的线程库
target_link_libraries(m_back pthread)
#zdb  数据库连接池库
target_link_libraries(m_back zdb)
#https  stl openssl依赖库
target_link_libraries(m_back ssl crypto)
#dl
target_link_libraries(m_back dl)
