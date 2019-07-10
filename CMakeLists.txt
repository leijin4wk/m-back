cmake_minimum_required(VERSION 3.5)
project(m_back C)

set(CMAKE_C_STANDARD 99)

MESSAGE(STATUS "Current Operation System: ${CMAKE_SYSTEM}")

MESSAGE(STATUS "Current Operation System Name: ${CMAKE_SYSTEM_NAME}")

MESSAGE(STATUS "Current Operation System Name: ${CMAKE_SYSTEM_PROCESSOR}")

include_directories(/octlr/include)

link_directories(/octlr/lib)

add_definitions(-D_GNU_SOURCE)
#条件编译
option (KORE_DEBUG  "DEBUG" ON)
if (KORE_DEBUG)
    message("DEBUG OPEN")
    add_definitions(-DKORE_DEBUG)
endif ()
##
# 查找目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(src/ DIR_SRCS)
add_executable(m_back ${DIR_SRCS})
#pthread  posix的线程库
#m  math库
target_link_libraries(m_back pthread m http_parser)