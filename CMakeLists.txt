cmake_minimum_required(VERSION 3.10)

# project setting
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 生成可执行文件
add_executable(ChessGame
    src/main.cpp
    src/ChineseChess.cpp
)

# 包含头文件目录
target_include_directories(ChessGame PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)