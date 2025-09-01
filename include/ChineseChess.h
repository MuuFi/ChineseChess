#ifndef CHINESECHESS_H
#define CHINESECHESS_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <utility>
#include <unordered_map>
#include <memory>
#include <functional>
#include <array>

enum COLOR {
    RED_WAY = true, BLACK_WAY = false
};

struct Pos {
    int x, y;
    inline bool operator==(const Pos& other) const { return x == other.x && y == other.y; }
};
namespace std {
    template <>
    struct hash<Pos> {
        inline std::size_t operator()(const Pos& p) const noexcept {
            return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
        }
    };
}
// using
using String = std::string;
using VecPos = std::vector<Pos>;
using Offset = std::unordered_map<bool, std::vector<Pos>>;

// 基础结构的向前声明
struct Board;
struct Pieces;
struct Pawns;
struct Rooks;
struct Knights;
struct Cannons;
struct Elephants;
struct Mandarins;
struct General;

// 棋子
struct Pieces {
    virtual ~Pieces() = default;
    virtual VecPos Predict(const Board& broad) = 0;
    virtual String getPieceName() = 0;

    static bool inBound(const Pos& p);
    static bool inBoundary(bool c, const Pos& p);
    static bool inCamp(bool c, const Pos& p);
    Pos Position;         // 记录棋子位置
    bool color;           // 记录棋子所属方向

    Pieces(const Pos& p, bool isRed);
};
// 兵/卒
struct Pawns: public Pieces {
    Pawns(const Pos& p, bool isRed) : Pieces(p, isRed) {}
    ~Pawns();
    VecPos Predict(const Board& broad) override;
    String getPieceName() override;
};
// 车
struct Rooks: public Pieces {
    Rooks(const Pos& p, bool isRed) : Pieces(p, isRed) {}
    ~Rooks();
    VecPos Predict(const Board& broad) override;
    String getPieceName() override;
};
// 马
struct Knights: public Pieces {
    Knights(const Pos& p, bool isRed) : Pieces(p, isRed) {}
    ~Knights();
    VecPos Predict(const Board& broad) override;
    String getPieceName() override;
};
// 炮
struct Cannons: public Pieces {
    Cannons(const Pos& p, bool isRed) : Pieces(p, isRed) {}
    ~Cannons();
    VecPos Predict(const Board& broad) override;
    String getPieceName() override;
};
// 相/象
struct Elephants: public Pieces {
    Elephants(const Pos& p, bool isRed) : Pieces(p, isRed) {}
    ~Elephants();
    VecPos Predict(const Board& broad) override;
    String getPieceName() override;
};
// 士/仕
struct Mandarins: public Pieces {
    Mandarins(const Pos& p, bool isRed) : Pieces(p, isRed) {}
    ~Mandarins();
    VecPos Predict(const Board& broad) override;
    String getPieceName() override;
};
// 帅/将
struct General: public Pieces {
    General(const Pos& p, bool isRed) : Pieces(p, isRed) {}
    ~General();
    VecPos Predict(const Board& broad) override;
    String getPieceName() override;
};

// 进阶结构的向前声明
struct Move;
struct Board;
class Player;
class XiangqiGame;

// using
using VecXQ = std::vector<std::unique_ptr<Pieces>>;
using PosMap = std::unordered_map<Pos, std::unique_ptr<Pieces>>;

struct Move {
    Pos from;
    Pos to;
    bool color;

    Move();
    ~Move() = default;

    void serialize(std::ofstream sf);
    void deserialize(std::ifstream lf);
    
    friend std::istream& operator>>(std::istream& is, Move& move);
};
// 棋盘
struct Board {
    PosMap situation;               // 当前棋盘布局
    static PosMap initializer();

    bool move(const Move& moveData);// 兼顾移动和吃子
    Board();
    ~Board() = default;
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
    Player gamePlayerRed; // 红方
    Player gamePlayerBla; // 黑方
    Board board;          // 棋盘
    bool nowTurn = 1;
    bool gameFinish = false;
    // ADD: 增加一些成员变量
public:
    XiangqiGame();
    XiangqiGame(const Player& p1, const Player& p2);
    
    Move getPlayerMove(bool currPlayer);
    bool isValid(const Move& moveData);
    void executeMove(const Move& moveData);
    bool checkMate(const Move& lastMove);
    void GamePlay();
    
    const Board& GetBoardInfo() const;
};

#endif // CHINESECHESS_H