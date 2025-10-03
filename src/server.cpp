#include <stdint.h>
#include <stdio.h>
#include <thread>
#include <atomic>
#include <memory>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "boost/beast/core.hpp"
#include "Chess_i.h"
#include "ChineseChess.h"

constexpr static size_t RecvMsgBufferSize = 256;

struct Server::server_impl {
    WSADATA wsaData;
    SOCKET server_fd;
    SOCKET client_fd;
    sockaddr_in address;
    sockaddr client_addr;
    int port;
    String lastErrMsg;
    int lastErrCode;

    std::thread recvThread;
    char MsgBuf[RecvMsgBufferSize];// recvThread recv()专用
    Move MovBuf;
    String lastMsg;
    bool running;
};


// 初始化，绑定端口以及监听
void Server::initServer() {
    String& ErrMsg = pimpl->lastErrMsg;
    int& ErrCode = pimpl->lastErrCode;

    // 初始化WSA
    if (WSAStartup(MAKEWORD(2,2), &(pimpl->wsaData)) != 0) {
        ErrMsg = "WSAStartup failed.";
        ErrCode = -1;
    }
    // 初始化SOCKET
    pimpl->server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (pimpl->server_fd == INVALID_SOCKET) {
        ErrMsg = "socket failed: " + std::to_string(WSAGetLastError());
        ErrCode = -2;
    }

    // 初始化addr
    pimpl->address.sin_family = AF_INET;         // 协议类型
    pimpl->address.sin_addr.s_addr = INADDR_ANY; // IP, 监听所有IP
    pimpl->address.sin_port = htons(pimpl->port);// 设置端口

    // 绑定addr
    if (bind(pimpl->server_fd, (sockaddr*)&(pimpl->address), sizeof(pimpl->address)) == SOCKET_ERROR) {
        ErrMsg = "bind failed: " + std::to_string(WSAGetLastError());
        ErrCode = -3;
    }
    // 监听连接
    if(listen(pimpl->server_fd, 5) == SOCKET_ERROR) {
        ErrMsg = "listen_init failed: " + std::to_string(WSAGetLastError());
        ErrCode = -4;
    }
}
// 错误处理
void Server::initServErrProc(int errCode) {
    switch (errCode)
    {
    case 0:
        std::cout << "connected successfully" << std::endl;
        return;
    case -1:
    case -2:
    case -3:
    case -4:
    case -5:
    default:
        break;
    }
}
// 接收消息
String Server::recvMsg() {
    char (&buffer)[RecvMsgBufferSize] = pimpl->MsgBuf;// 创建一个 buffer 别名优化一下可读性
    memset(buffer, 0, RecvMsgBufferSize);
    int len = recv(pimpl->client_fd, buffer, RecvMsgBufferSize-1, 0);
    if (len > 0) { return String(buffer, len); }
    return String();
}


// 构造析构
Server::Server() {
    pimpl = new server_impl();
    pimpl->port = 8080;

    initServer();
    acceptClient();
    initServErrProc(pimpl->lastErrCode);

    pimpl->running = true;
}
Server::~Server() {
    pimpl->running = false;
    if (pimpl->recvThread.joinable()) { pimpl->recvThread.join(); }
    closesocket(pimpl->server_fd);
    WSACleanup();
    delete pimpl;
}

void Server::sendMove(const Move& sendData) {
    // need serialize
    // need to save the data to SSD
}
Move Server::recvMove() {
    /* code */
    return Move();
}

void Server::sendMsg(const String& msg) {
    send(pimpl->client_fd, msg.c_str(), msg.size(), 0);
}
void Server::recvThreadFunc() {
    String& msg = pimpl->lastMsg;                     // msg 是 pimpl->lastMsg 的别名
    char (&buffer)[RecvMsgBufferSize] = pimpl->MsgBuf;// buffer 是 pimpl->MsgBuf 的别名
    
    while(pimpl->running) {
        memset(buffer, 0, RecvMsgBufferSize);         // 不必要的操作，只是为了安全和调试方便
        int len = recv(pimpl->client_fd, buffer, RecvMsgBufferSize-1, 0); // 留一个 '\0' 防止溢出
        if (len > 0) { msg = String(buffer, len); }   // 传入 lastMsg
        if (len == 0) { 
            // 连接中断
            pimpl->client_fd = INVALID_SOCKET;
            pimpl->running = false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));       // 留 50ms 处理
    }
}
String& Server::getLastMsg() {
    return pimpl->lastMsg;
}

// 初始化线程
void Server::startRecvThread() {
    pimpl->recvThread = std::thread(&Server::recvThreadFunc, this);
}

// 换成检查套接字的实现
bool Server::isConnected() {
    return pimpl->client_fd != INVALID_SOCKET;
}
// 接收连接
void Server::acceptClient() {
    String& ErrMsg = pimpl->lastErrMsg;
    int& ErrCode = pimpl->lastErrCode;

    int addrlen = sizeof(pimpl->client_addr);
    pimpl->client_fd = accept(pimpl->server_fd, &(pimpl->client_addr), (socklen_t*)&addrlen);
    if(pimpl->client_fd == INVALID_SOCKET) {
        ErrMsg = "connecting err, code:" + std::to_string(WSAGetLastError());
        ErrCode = -5;
    }
}
// 
void Server::setPort(int newPort) {
    if (newPort == pimpl->port) { return; }

    // 停止服务
    pimpl->running = false;
    if(pimpl->recvThread.joinable()) { pimpl->recvThread.join(); }
    if(pimpl->client_fd != INVALID_SOCKET) { closesocket(pimpl->client_fd); }
    if(pimpl->server_fd != INVALID_SOCKET) { closesocket(pimpl->server_fd); }

    // 设置新端口
    pimpl->port = newPort;

    // 再次初始化
    initServer();
    acceptClient();
    initServErrProc(pimpl->lastErrCode);// 老实说这玩意意义不明

    // 重启线程
    pimpl->running = true;
    startRecvThread();
}

// getErr
String Server::getLastErrMsg() const {
    return pimpl->lastErrMsg;
}
int Server::getLastErrCode() const {
    return pimpl->lastErrCode;
}
