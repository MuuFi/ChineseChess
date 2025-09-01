#ifndef SERVER_H
#define SERVER_H

#include <stdint.h>
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "boost/beast/core.hpp"
#include "ChineseChess.h"
#pragma comment(lib, "ws2_32.lib")


class Server{
private:
    /* data */
    WSADATA wsaData;
    SOCKET server_fd;
    SOCKET client_fd;
    sockaddr_in address;

    sockaddr client_addr;// 用于异常检查
    int port;

    unsigned int status;

    int initialize();// Q：是不是分开更好点
    void cleanUp();
public:
    Server();
    ~Server();

    Move recvMove();
    void sendMove(const Move& sendData);
};

int Server::initialize() {
    int checkError = 0;
    // 设置端口
    port = 8080;
    // 初始化WSA
    if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0) { std::cout << "WSAStartup failed." << std::endl; return -1; }
    // 初始化SOCKET
    server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(server_fd == INVALID_SOCKET) { std::cout << "socket failed: " << WSAGetLastError() << std::endl; return -2; }
    // 初始化addr
    address.sin_family = AF_INET;        // 协议类型
    address.sin_addr.s_addr = INADDR_ANY;// IP, 监听所有IP
    address.sin_port = htons(port);      // 设置端口
    // 绑定addr
    checkError = bind(server_fd, (sockaddr*)&address, sizeof(address));
    if(checkError = SOCKET_ERROR) { std::cout << "bind failed: " << WSAGetLastError() << std::endl; return -3; }
    // 监听连接
    checkError = listen(server_fd, 5);
    if(checkError = SOCKET_ERROR) { std::cout << "listen_init failed: " << WSAGetLastError() << std::endl; return -4; }
    // 接受连接
    client_fd = accept(client_fd, &client_addr, nullptr);
    if(client_fd == INVALID_SOCKET) { std::cout << "maybe connecting err" << WSAGetLastError() << std::endl; return -5; }
    // 连接结束
    return 0;
}
void Server::cleanUp() {
    close(server_fd);
    WSACleanup();
}

// 构造析构
Server::Server() {
    // 按我这种写法，构造成功相当于服务起来了

    int errorCode = initialize();
    if(errorCode < 0) { std::cout << "An anomaly occurs" << std::endl; }
    else switch (errorCode)
    {
    case 0:
        std::cout << "connected sucessfully" << std::endl;
        break;
    case -1:
    case -2:
    case -3:
    case -4:
    case -5:
        // 可以做一点错误处理，不过不知道咋写了
        break;
    default:
        
        break;
    }
}
Server::~Server() {
    cleanUp();
}

Move Server::recvMove() {
    /* code */
    return Move();
}
void Server::sendMove(const Move& sendData) {
    // need serialize
    // need to save the data to SSD
}
#endif