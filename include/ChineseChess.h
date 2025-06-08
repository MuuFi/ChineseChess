#ifndef CHINESECHESS_H
#define CHINESECHESS_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <utility>
#include <unordered_map>

// 数据结构的向前声明
struct XiangqiPiece;
class Player;
class XiangqiGame;

// using
using Pos = std::pair<int, int>;
using VecXQ = std::vector<XiangqiPiece>;
using String = std::string;
using PosMap = std::unordered_map<std::pair<int, int>, XiangqiPiece>;

// 棋子
struct XiangqiPiece {
    String type;          // 区分棋子类型
    Pos Position;         // 记录棋子位置
    bool color;           // 记录棋子所属方向
    XiangqiPiece(String str, const Pos& p, bool isRed);
};
// 玩家
class Player {
private:
    String name;       // 用户名
    bool way;          // 用户的方向，是否是红方
public:
    Player();                                        // 默认构造
    Player(String str);                              // 输入用户名
    Player(std::ifstream& initFile);                 // 调用 deserialize() 从文件读取

    void serialize(std::ofstream& sf);               // 把类数据序列化到文件
    void deserialize(std::ifstream& lf);             // 从文件读取序列化到数据
};
// Game
class XiangqiGame {
private:
    Player gamePlayer[2];            // 用户
    VecXQ allPieces;                 // 所有棋子的数据
    bool pieceExist[9][10];          // 记录某一位置是否有棋子
    const Pos broadSize = {9, 10};      // 棋盘大小
    static const VecXQ apInitData;      // 储存所有棋子的初始位置数据
    static const bool peInitData[9][10];// 在所有棋子初始位置对应的坐标上写 true，其他地方写 false
public:
    XiangqiGame();
    XiangqiGame(const Player& p1, const Player& p2);

    std::vector<Pos> predicted(size_t pieceIdx) const;
    void move(size_t pieceIdx, const Pos& target);
};
#endif