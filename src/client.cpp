#include <stdint.h>
#include <stdio.h>
#include <thread>
#include <atomic>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "boost/beast/core.hpp"
#include "Chess_i.h"
#include "ChineseChess.h"

struct Client::client_impl {
    WSADATA wsaData;
    SOCKET  clientfd;
    sockaddr_in serverAddr;

    std::thread recvThread;
    std::thread sendThread;
    bool connecting;
    String MsgBuf;
    Move MovBuf;
};



int Client::initializeConnect(const char* server_ip, int port) {
    pimpl = new client_impl();
    // 初始化WSA
    if (WSAStartup(MAKEWORD(2,2), &(pimpl->wsaData)) != 0) return -1;
    // 创建socket
    pimpl->clientfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (pimpl->clientfd == INVALID_SOCKET) return -2;
    // 设置服务器地址
    pimpl->serverAddr.sin_family = AF_INET;
    pimpl->serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, server_ip, &(pimpl->serverAddr.sin_addr));
    // 连接服务器
    if (connect(pimpl->clientfd, (sockaddr*)&(pimpl->serverAddr), sizeof(pimpl->serverAddr)) == SOCKET_ERROR) return -3;
    return 0;
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


Client::Client() {
    int errorCode = 0;
    errorCode = initializeConnect("127.0.0.1", 8080);
    initConnErrProc(errorCode);
}

Client::~Client() {
    closesocket(pimpl->clientfd);
    WSACleanup();
    delete pimpl;
}



void Client::sendMove(const Move& sendData) {
    // TODO
}

Move Client::recvMove() {
    // TODO
    return Move();
}

void Client::sendMsg(const String& msg) {
    send(pimpl->clientfd, msg.c_str(), msg.size(), 0);
}

String Client::recvMsg() {
    char buffer[1024] = {0};
    int len = recv(pimpl->clientfd, buffer, sizeof(buffer)-1, 0);
    if(len > 0) { return String(buffer, len); }
    return String();
}