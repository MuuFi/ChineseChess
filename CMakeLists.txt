cmake_minimum_required(VERSION 3.10)

# project setting
project(ChessGame)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# raylib 配置
set(RAYLIB_ROOT "D:\\IWA\\LIB\\raylib-5.5_win64_mingw-w64")
find_path(RAYLIB_INCLUDE_DIR raylib.h PATHS ${RAYLIB_ROOT}/include REQUIRED)
find_library(RAYLIB_LIBRARY NAMES raylib PATHS ${RAYLIB_ROOT}/lib REQUIRED)

# 创建 raylib 导入目标
add_library(raylib STATIC IMPORTED)
set_target_properties(raylib PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${RAYLIB_INCLUDE_DIR}"
    IMPORTED_LOCATION "${RAYLIB_LIBRARY}"
)

# boost 配置
set(BOOST_ROOT "D:/IWA/LIB/boost_1_88_0")
find_path(BOOST_INCLUDE_DIR boost/system.hpp PATHS ${BOOST_ROOT} REQUIRED)
find_library(BOOST_SYSTEM_LIBRARY
    NAMES libboost_system-mgw15-mt-x64-1_88
    PATHS ${BOOST_ROOT}/stage/lib REQUIRED
)

add_library(Boost::system STATIC IMPORTED)
set_target_properties(Boost::system PROPERTIES
    IMPORTED_LOCATION "${BOOST_SYSTEM_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${BOOST_INCLUDE_DIR}"
)

# 生成可执行文件
add_executable(ChessGame
    src/main.cpp
    src/ChineseChess.cpp
)

# 包含头文件目录
target_include_directories(ChessGame PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${RAYLIB_ROOT}/include
    ${BOOST_INCLUDE_DIR}
)

# 链接库
target_link_libraries(ChessGame PRIVATE
    raylib
    winmm          # raylib在Windows上的依赖
    gdi32          # raylib在Windows上的依赖
    Boost::system
)