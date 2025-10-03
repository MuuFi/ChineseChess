// 在此实现graphic.cpp和server.cpp分别的接口
// 大致就是合并 graphic.h 和 server.h 的声明，然后各自定义在 cpp 里
// 很简单
#ifndef CHESS_I_H
#define CHESS_I_H
#include "ChineseChess.h"

class ChessUI {
private:
    const Board* observer;
    bool mouseDown;

public:
    ChessUI(const Board* board_ptr);
    ~ChessUI();
    void clickCheck();

};
class ChessGUI
{
private:
    struct graphic_impl;
    graphic_impl* pimpl;

    void initGraphic();
    void DrawChineseChessBoard(int X, int Y, int cellW, int cellH, int cols, int rows);
    void DrawPieces(int posX, int posY);
    ChessGUI(const ChessGUI&) = delete;
    ChessGUI operator=(const ChessGUI&) = delete;
public:
    ChessGUI(const Board* board_ptr);
    ~ChessGUI();
    void clickCheckHandler();
    void renderUpdated();
    void renderPieces(const Board& situation, bool isFontLoaded);
    bool ExitGame();
};



class Server{
private:
    struct server_impl;
    server_impl* pimpl;


    void initServer();
    void initServErrProc(int errCode);
    String recvMsg(void);

public:
    Server();
    ~Server();

    void sendMove(const Move& sendData);
    Move recvMove(void);

    void sendMsg(const String& msg);
    void recvThreadFunc();
    String& getLastMsg();
    void startRecvThread();
    

    bool isConnected();
    void acceptClient();
    void setPort(int newPort);

    String getLastErrMsg() const;
    int getLastErrCode() const;
};
class Client {
private:
    struct client_impl;
    client_impl* pimpl;

    int initializeConnect(const char* server_ip, int port);
    void initConnErrProc(int errCode);
public:
    Client();
    ~Client();

    void sendMove(const Move& sendData);
    Move recvMove(void);

    void sendMsg(const String& msg);
    String recvMsg(void);
};

#endif