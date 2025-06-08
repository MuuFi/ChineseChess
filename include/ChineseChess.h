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
using TypeMap = std::unordered_map<char, std::string>;

// 棋子
struct XiangqiPiece {
    char type;            // 区分棋子类型
    Pos Position;         // 记录棋子位置
    bool color;           // 记录棋子所属
    XiangqiPiece(char ch);
    XiangqiPiece(char ch, const Pos& p);
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
    Player gamePlayer[2];          // 用户
    VecXQ allPieces;               // 所有棋子的数据
    const Pos broadSize = {9, 10}; // 棋盘大小
    static const VecXQ apInitData; // 储存所有棋子的初始位置数据
    static const TypeMap typeName; // 储存抽象的char记录类型到直观的棋子类型的映射
public:
    XiangqiGame();
    XiangqiGame(const Player& p1, const Player& p2);

    std::vector<Pos> predicted(size_t pieceIdx) const;
    void move(size_t pieceIdx, const Pos& target);
};
#endif