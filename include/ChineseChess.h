#ifndef CHINESECHESS_H
#define CHINESECHESS_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <utility>

using Pos = std::pair<int, int>;
using VecXQ = std::vector<XiangqiPiece>;
using String = std::string;

struct XiangqiPiece {
    char type;            // 区分棋子类型
    Pos Position;         // 记录棋子位置
    XiangqiPiece(char ch);
    XiangqiPiece(char ch, const Pos& p);
};
class Player {
private:
    String name;       // 用户名
    VecXQ retainPieces;// 用户保有的棋子

public:
    Player();                                        // 默认构造
    Player(String str);                              // 输入用户名
    Player(String str, const VecXQ& initialize);     // 输入用户名和用户保有的棋子构造
    Player(std::ifstream& initFile);                 // 调用 deserialize() 从文件读取

    void serialize(std::ofstream& sf);               // 把类数据序列化到文件
    void deserialize(std::ifstream& lf);             // 从文件读取序列化到数据
    std::vector<Pos> predicted(size_t PieceIdx);     // 根据索引从 VecXQ 中读取对应的棋子然后根据棋子的 type 预测他的落点 
    void move(size_t PieceIdx, const Pos& targetPos);// 把索引对应的棋子移动到目标位置(targetPos)
};
class XiangqiGame {
private:
    Player gamePlayer[2];// 用户
    int board[9][10];    // 棋盘
public:
    XiangqiGame();
};
#endif