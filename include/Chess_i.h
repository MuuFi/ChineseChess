/* 声明接口
*  ChessUI, ChessGUI -> graphic.cpp
*  Server            -> server.cpp
*  Client            -> client.cpp
*/

#ifndef CHESS_I_H
#define CHESS_I_H
#include "ChineseChess.h"

class ChessGUI
{
private:
    struct ChessGUI_Impl;
    ChessGUI_Impl* pimpl;

    // include this↓ into constructorFunc
    void initGraphic();

    void DrawChineseChessBoard(int X, int Y, int cellW, int cellH, int cols, int rows);
    void DrawPieces(int posX, int posY, Pieces* p);
    std::vector<uint32_t> parseUTF8(const String& str);

    ChessGUI(const ChessGUI&) = delete;
    ChessGUI operator=(const ChessGUI&) = delete;
public:
    ChessGUI(const Board& ob);
    ~ChessGUI();

    void renderUpdated();                      // tobe replaced
    void renderPieces(const Board& situation); // tobe replaced
    bool ExitGame();

    /* Design impl */
    // input the pos of UpLeft of Board
    void BoardRenderHandle(int X, int Y); // use to render the Board, only need to call one time
    void PieceRenderHandle(int X, int Y); // use to render the Pieces from ui.seeBoard()
    /* end Design */

    void clickCheck();
};



class Server{
private:
    struct server_impl;
    server_impl* pimpl;


    void initServer();
    void initServErrProc(int errCode);
public:
    Server();
    ~Server();

    void sendMove(const Move& sendData);
    void recvMovThreadFunc(XiangqiGame& game);
    void startRecvMov(XiangqiGame& game);

    void sendMsg(const String& msg);
    void recvMsgThreadFunc();
    String& getLastMsg();
    void startRecvThread();


    bool isConnected();
    void acceptClient();
    void closeServer();
    void setPort(int newPort);

    String getLastErrMsg() const;
    int getLastErrCode() const;
};
class Client {
private:
    struct client_impl;
    client_impl* pimpl;

    void initClient();
    void initConnErrProc(int errCode);
public:
    Client();
    ~Client();

    void sendMove(const Move& sendData);
    void recvMovThreadFunc(XiangqiGame& game);
    void startRecvMov(XiangqiGame& game);

    void sendMsg(const String& msg);
    void recvThreadFunc();
    String& getLastMsg();
    void startRecvThread();

    bool isConnected();
    void connServer();
    void closeConn();

    String getLastErrMsg() const;
};

#endif