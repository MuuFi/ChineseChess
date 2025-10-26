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

constexpr static size_t RecvMsgBufSize = 256;
constexpr static size_t RecvMovBufSize = 4;  // 其实可以用uint8_t，但暂时先这样

struct Server::server_impl {
    WSADATA wsaData;
    SOCKET server_fd;
    SOCKET client_fd;
    sockaddr_in address;
    sockaddr client_addr;
    int port;
    String lastErrMsg;
    int lastErrCode;

    std::thread recvMsgThread;
    char MsgBuf[RecvMsgBufSize];// recvMsgThread 专用

    std::thread recvMovThread;
    char MovBuf[RecvMovBufSize];

    String lastMsg;
    bool hasNewMsg;
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



// 构造析构
Server::Server() {
    pimpl = new server_impl();
    pimpl->port = 8080;
    pimpl->lastErrCode = 0;

    initServer();
    acceptClient();
    initServErrProc(pimpl->lastErrCode);

    pimpl->running = true;
}
Server::~Server() {
    pimpl->running = false;
    if (pimpl->recvMsgThread.joinable()) { pimpl->recvMsgThread.join(); }
    if (pimpl->recvMovThread.joinable()) { pimpl->recvMovThread.join(); }
    closesocket(pimpl->server_fd);
    WSACleanup();
    delete pimpl;
}

// send/recv Mov
void Server::sendMove(const Move& sendData) {
    static const int DataBytesSize = 4;
    int data = htonl(sendData.compressMove());
    const char* buffer = reinterpret_cast<const char*>(&data);
    send(pimpl->client_fd, buffer, DataBytesSize, 0);
}
void Server::recvMovThreadFunc(XiangqiGame& game) {
    char (&buffer)[RecvMovBufSize] = pimpl->MovBuf;
    Move mov;
    while(pimpl->running) {
        int len = recv(pimpl->client_fd, buffer, RecvMovBufSize, 0);
        if (len > 0) {
            mov = Move(ntohl(*reinterpret_cast<int*>(buffer)));
            movRequest::movFromINET(game, mov);
        }
        if (len == 0) {
            pimpl->client_fd = INVALID_SOCKET;
            pimpl->running = false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
void Server::startRecvMov(XiangqiGame& game) {
    pimpl->recvMovThread = std::thread(&Server::recvMovThreadFunc, this, std::ref(game));
}

// send/recv Msg
void Server::sendMsg(const String& msg) {
    send(pimpl->client_fd, msg.c_str(), msg.size(), 0);
}
void Server::recvMsgThreadFunc() {
    String& msg = pimpl->lastMsg;                     // msg 是 pimpl->lastMsg 的别名
    char (&buffer)[RecvMsgBufSize] = pimpl->MsgBuf;// buffer 是 pimpl->MsgBuf 的别名
    
    while(pimpl->running) {
        memset(buffer, 0, RecvMsgBufSize);         // 不必要的操作，只是为了安全和调试方便
        int len = recv(pimpl->client_fd, buffer, RecvMsgBufSize-1, 0); // 留一个 '\0' 防止溢出
        if (len > 0) {
            msg = String(buffer, len);
            pimpl->hasNewMsg = true;
        }
        if (len == 0) { 
            // 连接中断
            pimpl->client_fd = INVALID_SOCKET;
            pimpl->running = false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));       // 留 50ms 处理
    }
}
String& Server::getLastMsg() {
    pimpl->hasNewMsg = false;
    return pimpl->lastMsg;
}
void Server::startRecvThread() {
    pimpl->recvMsgThread = std::thread(&Server::recvMsgThreadFunc, this);
}

// 检查套接字的实现
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
// 重置端口
void Server::setPort(int newPort) {
    if (newPort == pimpl->port) { return; }

    // 停止服务
    pimpl->running = false;
    if(pimpl->recvMsgThread.joinable()) { pimpl->recvMsgThread.join(); }
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
