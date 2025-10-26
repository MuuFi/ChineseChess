#include <stdint.h>
#include <stdio.h>
#include <thread>
#include <atomic>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "boost/beast/core.hpp"
#include "Chess_i.h"
#include "ChineseChess.h"

static constexpr size_t RecvMsgBufSize = 256;
static constexpr size_t RecvMovBufSize = 4;

struct Client::client_impl {
    WSADATA wsaData;
    SOCKET  client_fd;
    sockaddr_in serverAddr;
    String serverIP;
    int serverPort;
    String lastErrMsg;
    int lastErrCode;

    std::thread recvMsgThread;
    char MsgBuf[RecvMsgBufSize];
    std::thread recvMovThread;
    char MovBuf[RecvMovBufSize];
    String lastMsg;
    bool hasNewMsg;
    bool connecting;
};


// 启动客户端，不连接
void Client::initClient() {
    String& IP = pimpl->serverIP;
    int& port  = pimpl->serverPort;

    String& errMsg = pimpl->lastErrMsg;
    int& errCode = pimpl->lastErrCode;

    // 初始化WSA
    if (WSAStartup(MAKEWORD(2,2), &(pimpl->wsaData)) != 0) { errCode = -1; }
    // 初始化Client
    pimpl->client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);// 创建SOCKET
    if (pimpl->client_fd == INVALID_SOCKET) { errCode = -2; }// 检查是否创建失败
    // 读取服务器地址
    pimpl->serverAddr.sin_family = AF_INET;
    pimpl->serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, IP.c_str(), &(pimpl->serverAddr.sin_addr));
}
void Client::initConnErrProc(int errCode) {
    switch (errCode)
    {
    case 0:
        std::cout << "connected successfully" << std::endl;
        return;
    case -1:
    case -2:
    case -3:
    default:
        return;
    }
}

// 
Client::Client() {
    pimpl = new client_impl();
    pimpl->serverIP = "127.0.0.1";
    pimpl->serverPort = 8080;
    pimpl->lastErrCode = 0;
    pimpl->hasNewMsg = false;

    initClient();
    connServer();
    initConnErrProc(pimpl->lastErrCode);
}
Client::~Client() {
    pimpl->connecting = false;
    closesocket(pimpl->client_fd);
    if (pimpl->recvMsgThread.joinable()) { pimpl->recvMsgThread.join(); }
    if (pimpl->recvMovThread.joinable()) { pimpl->recvMovThread.join(); }
    WSACleanup();
    delete pimpl;
}



// send/recv Mov
void Client::sendMove(const Move& sendData) {
    const static int DataBytesSize = 4;
    int data = htonl(sendData.compressMove());
    const char* buffer = reinterpret_cast<const char*>(&data);
    send(pimpl->client_fd, buffer, DataBytesSize, 0);
}
void Client::recvMovThreadFunc(XiangqiGame& game) {
    char (&buffer)[RecvMovBufSize] = pimpl->MovBuf;
    Move mov;

    while(pimpl->connecting) {
        int len = recv(pimpl->client_fd, buffer, RecvMovBufSize, 0);
        if (len > 0) {
            mov = ntohl(*reinterpret_cast<int*>(buffer));
            movRequest::movFromINET(game, mov);
        }
        if (len == 0) {
            pimpl->client_fd = INVALID_SOCKET;
            pimpl->connecting = false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
void Client::startRecvMov(XiangqiGame& game) {
    pimpl->recvMovThread = std::thread(&Client::recvMovThreadFunc, this, std::ref(game));
}

// send/recv Msg
void Client::sendMsg(const String& msg) {
    send(pimpl->client_fd, msg.c_str(), msg.size(), 0);
}
void Client::recvThreadFunc() {
    char (&buffer)[RecvMsgBufSize] = pimpl->MsgBuf;// 别名
    String& msg = pimpl->lastMsg;                  // 别名

    while(pimpl->connecting) {                     // 检查连接状态
        // 没有清空 buffer
        int len = recv(pimpl->client_fd, buffer, RecvMsgBufSize-1, 0);// 以 buffer 为缓冲区接收消息
        if (len > 0) { msg = String(buffer, len); }
        if (len == 0) {
            // 连接中断处理
            pimpl->connecting = false;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));// 每隔 50ms 执行
    }
}
String& Client::getLastMsg() {
    return pimpl->lastMsg;
}
void Client::startRecvThread() {
    pimpl->recvMsgThread = std::thread(&recvThreadFunc, this);
}

// Connect
bool Client::isConnected() {
    return pimpl->client_fd != INVALID_SOCKET;
}
void Client::connServer() {
    if (connect(pimpl->client_fd, (sockaddr*)&(pimpl->serverAddr), sizeof(pimpl->serverAddr)) == SOCKET_ERROR) {
        pimpl->lastErrCode = -3;
    }
}