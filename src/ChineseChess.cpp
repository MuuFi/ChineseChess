#include "ChineseChess.h"

// using
using VecXQ = std::vector<std::unique_ptr<Pieces>>;
using String = std::string;
using PosMap = std::unordered_map<Pos, std::unique_ptr<Pieces>>;
using Offset = std::unordered_map<bool, std::vector<Pos>>;
using VecPos = std::vector<Pos>;

// Pieces
Pieces::Pieces(const Pos &p, bool isRed) : Position(p), color(isRed) {}
bool Pieces::inBound(const Pos &p) {
    return (p.x >= 0) && (p.x <= 8) && (p.y >= 0) && (p.y <= 9);
}
bool Pieces::inBoundary(bool c, const Pos &p) {
    return c == COLOR::RED_WAY ?
                               // 红方"领地"
               (p.x >= 0) && (p.x <= 8) &&
                   (p.y >= 0) && (p.y <= 4)
                               :
                               // 黑方"领地"
               (p.x >= 0) && (p.x <= 8) &&
                   (p.y >= 5) && (p.y <= 9);
}
bool Pieces::inCamp(bool c, const Pos &p) {
    return c == COLOR::RED_WAY ?
                               // 红方帅帐
               (p.x >= 3) && (p.x <= 5) &&
                   (p.y >= 0) && (p.y <= 2)
                               :
                               // 黑方将帐
               (p.x >= 3) && (p.y <= 5) &&
                   (p.y >= 7) && (p.y <= 9);
}
// Pawns
Pawns::~Pawns() {}
VecPos Pawns::Predict(const Board &board) {
    VecPos targets;
    const Pos &p = Position;
    // 过河后的移动
    static const Offset RIVER_OFFSETS = {
        {COLOR::RED_WAY, {{-1, 0}, {0, 1}, {1, 0}}},
        {COLOR::BLACK_WAY, {{-1, 0}, {0, -1}, {1, 0}}}};
    // 过河前的移动，按颜色索引
    static const Offset MOVE_OFFSETS = {
        {COLOR::RED_WAY, {{0, 1}}},
        {COLOR::BLACK_WAY, {{0, -1}}}};

    auto checkDir = [&](const Pos &destination)
    {
        if (!inBound(destination))
        {
            return false;
        }
        auto it = board.situation.find(destination);
        // 目标位置没有棋子，或者是敌人，则可以移动至目标位置
        return it == board.situation.end() || it->second->color != this->color;
    };
    // 判断棋子有没有过河
    // 红方纵坐标大于 4 算过河，黑方纵坐标小于 5 算过河
    bool isAcrossRiver = (color == COLOR::RED_WAY ? Position.y > 4 : Position.y < 5);
    // 应用状态模式
    if (isAcrossRiver)
    {
        const auto &offsets = RIVER_OFFSETS.at(color);
        for (const auto &offset : offsets)
        {
            Pos destination = {p.x + offset.x, p.y + offset.y};
            if (checkDir(destination))
            {
                targets.push_back(destination);
            }
        }
    }
    else
    {
        const auto &offsets = MOVE_OFFSETS.at(color);
        for (const auto &offset : offsets)
        {
            Pos destination = {p.x + offset.x, p.y + offset.y};
            if (checkDir(destination))
            {
                targets.push_back(destination);
            }
        }
    }
    return targets;
}
String Pawns::getPieceName() { return color == COLOR::RED_WAY ? "兵" : "卒"; }

// Rooks
Rooks::~Rooks() {}
VecPos Rooks::Predict(const Board &board) {
    VecPos targets;
    auto checkDir = [&](const Pos &destination, bool &locked)
    {
        if (locked || !inBound(destination))
        {
            return false;
        }
        // 先查找目标位置有无棋子
        auto it = board.situation.find(destination);

        // 如果没找到，则目标可到达，返回 true
        if (it == board.situation.end())
        {
            targets.push_back(destination);
            return true;
        }
        // 如果找到，则判断颜色，异色可达，同色则不达
        if (it->second->color != this->color)
        {
            targets.push_back(destination);
        }
        // 只要找到，一律锁住函数调用
        locked = true;
        return false; // 不用再 checkDir，返回 false
    };
    // 四个方向的偏移量，用计算时的 +、- 区分
    int d = 1;
    bool lock[4] = {false};
    for (Pos // 声明向四个方向递进的坐标
             RightWay = {Position.x + d, Position.y},
             LeftWay = {Position.x - d, Position.y},
             UpWay = {Position.x, Position.y + d},
             Downway = {Position.x, Position.y - d};
         // 当所有坐标都不在棋盘内时退出循环
         inBound(RightWay) || inBound(LeftWay) || inBound(UpWay) || inBound(Downway);
         ++d)
    {
        // 向周围坐标的检查停止于第一次遭遇到无法到达的坐标时。简单来讲：会被边界、敌方棋子的下一格或我方的棋子阻挡
        if (!lock[0])
        {
            lock[0] = !checkDir(RightWay, lock[0]);
        }
        if (!lock[1])
        {
            lock[1] = !checkDir(LeftWay, lock[1]);
        }
        if (!lock[2])
        {
            lock[2] = !checkDir(UpWay, lock[2]);
        }
        if (!lock[3])
        {
            lock[3] = !checkDir(Downway, lock[3]);
        }
    }
    return targets;
}
String Rooks::getPieceName() { return "車"; }

// Knights
Knights::~Knights() {}
VecPos Knights::Predict(const Board &board) {
    VecPos targets;
    const Pos &p = Position;   // 别名
    static const int foot = 1; // 马脚位的偏移
    static const int jump = 2; // 跳跃位的偏移

    // 八种情况，分别是 (x±f, y±j)、(x±j, y±f)

    // 在这里操作 targets
    auto AddTargets = [&](const Pos &target)
    {
        if (!inBound(target))
        {
            return;
        }
        // 先检查目标位置，然后分情况讨论。如果目标没有棋子或者异色(可吃)，就可达(加入targets)
        auto it = board.situation.find(target);
        if (it == board.situation.end() || it->second->color != this->color)
        {
            targets.push_back(target);
            return;
        }
        // 同色位不可达
        return;
    };
    // 如果什么都没找到，map 返回 end()
    auto NoPreventing = board.situation.end();
    // 可以到达的目标
    const Pos destination[8] = {
        {p.x + foot, p.y + jump}, // 0: 右上跳
        {p.x - foot, p.y + jump}, // 1: 左上跳
        {p.x + foot, p.y - jump}, // 2: 右下跳
        {p.x - foot, p.y - jump}, // 3: 左下跳
        {p.x + jump, p.y + foot}, // 4: 右跳上
        {p.x - jump, p.y + foot}, // 5: 左跳上
        {p.x + jump, p.y - foot}, // 6: 右跳下
        {p.x - jump, p.y - foot}  // 7: 左跳下
    };
    // 需要检查的马脚位
    const Pos check[4] = {
        {p.x + foot, p.y}, // 右马脚 (对应destination 0,2)
        {p.x - foot, p.y}, // 左马脚 (对应destination 1,3)
        {p.x, p.y + foot}, // 上马脚 (对应destination 4,5)
        {p.x, p.y - foot}  // 下马脚 (对应destination 6,7)
    };
    // 虽然循环也可以，但是要修改初始化顺序，而且反正最后都会展开不如就这样
    // 而且写都写了…………
    if (board.situation.find(check[0]) == NoPreventing)
    {
        if (inBound(destination[0]))
            targets.push_back(destination[0]);
        if (inBound(destination[2]))
            targets.push_back(destination[2]);
    }
    if (board.situation.find(check[1]) == NoPreventing)
    {
        if (inBound(destination[1]))
            targets.push_back(destination[1]);
        if (inBound(destination[3]))
            targets.push_back(destination[3]);
    }
    if (board.situation.find(check[2]) == NoPreventing)
    {
        if (inBound(destination[4]))
            targets.push_back(destination[4]);
        if (inBound(destination[5]))
            targets.push_back(destination[5]);
    }
    if (board.situation.find(check[3]) == NoPreventing)
    {
        if (inBound(destination[6]))
            targets.push_back(destination[6]);
        if (inBound(destination[7]))
            targets.push_back(destination[7]);
    }
    return targets;
}
String Knights::getPieceName() { return "马"; }

// Cannons
Cannons::~Cannons() {}
VecPos Cannons::Predict(const Board &board) {
    VecPos targets;
    const Pos &p = Position;
    static const VecPos dirs = {
        // 上
        {0, 1},
        // 下
        {0, -1},
        // 左
        {-1, 0},
        // 右
        {1, 0}};
    auto mark = [&](const Pos &destination, bool &stepped)
    {
        /* 返回 0 表示：目标不可达
         *  返回 1 表示：目标可达
         *  返回 2 表示：可以跳出循环 */
        // 目标不在棋盘，家里得请 哈基Break 了
        if (!inBound(destination))
        {
            return 2;
        }
        // 目标在棋盘上，开始进行一个开盒
        auto it = board.situation.find(destination);
        /* 展开if嵌套 */
        // 无炮架、无棋子时，可以到达
        if (!stepped && it == board.situation.end())
        {
            targets.push_back(destination);
            return 1;
        }
        // 无炮架、有棋子时，作为炮架，不可到达
        if (!stepped && it != board.situation.end())
        {
            stepped = true;
            return 0;
        }
        // 有炮架、有敌方棋子时，给老子炸
        if (stepped && it->second->color != this->color)
        {
            targets.push_back(destination);
            return 2;
        }
        // 有炮架，有我方棋子时，猪一样的队友
        if (stepped && it->second->color == this->color)
        {
            return 2;
        }
        // 有炮架，没旗子，啥也干不了
        return 0;
    };
    bool cross[4] = {false};
    for (size_t i = 0; i < 4; ++i)
    {
        int state = 1;
        // 初始化为相邻格，开始 遍历+标记
        for (Pos dest = {p.x + dirs[i].x, p.y + dirs[i].y};
             state != 2;
             dest.x += dirs[i].x, dest.y += dirs[i].y)
        {
            state = mark(dest, cross[i]);
        }
    }
    return targets;
}
String Cannons::getPieceName() { return "炮"; }

// Elephants
Elephants::~Elephants() {}
VecPos Elephants::Predict(const Board &board) {
    VecPos targets;
    static const VecPos checkDir = {
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    const Pos &p = Position;
    /* 加两次就到地方，到达目的地的偏移就不列举了 */
    // 用于检查象脚
    auto canReachBy = [&](const Pos &relatePos) {
        /* true : 可通过
         *  false: 不可通过
         */
        // 象脚不在棋盘上，通过个毛
        if (!inBound(relatePos)) {
            return false;
        }
        // 象脚在棋盘上，开始查成分
        auto it = board.situation.find(relatePos);
        // 没查到，象脚空出来了，可通过
        if (it == board.situation.end()) {
            return true;
        }
        // 查到了，别管是啥，总之别想过去
        return false;
    };
    auto Reach = [&](const Pos &destination)
    {
        // 目标不能在棋盘外，也不能在各方的边界外
        if (!inBound(destination))
        {
            return;
        }
        if (!inBoundary(this->color, destination))
        {
            return;
        }
        // 条件都满足。本关考验你，听声辩位功夫
        auto it = board.situation.find(destination);
        // 西南方：位置空出来的
        if (it == board.situation.end())
        {
            targets.push_back(destination);
            return;
        }
        else
        { /* 听不清楚 */
        }
        // 东方一个，东北方一个：虽然不是空的，但是是敌方棋子
        if (it->second->color != this->color)
        {
            targets.push_back(destination);
            return;
        }
        else
        { /* 该罚！*/
        }
        return /* 你 过 关 ！*/;
    };
    for (size_t i = 0; i < 4; ++i)
    {
        Pos checkPos = {p.x + checkDir[i].x, p.y + checkDir[i].y};
        if (canReachBy(checkPos))
        {
            checkPos.x += checkDir[i].x;
            checkPos.y += checkDir[i].y;
            Reach(checkPos);
        }
    }
    return targets;
}
String Elephants::getPieceName() { return color == COLOR::RED_WAY ? "相" : "象"; }

// Mandarins
Mandarins::~Mandarins() {}
VecPos Mandarins::Predict(const Board &board) {
    VecPos targets;
    const Pos &p = Position;
    static const VecPos dir = {
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    auto canReach = [&](const Pos &destination)
    {
        // 不在九宫之内，不可达
        if (!inCamp(this->color, destination))
        {
            return false;
        }
        // 在九宫之内，查成分
        auto it = board.situation.find(destination);
        // 位置 没棋子 或者 是敌方棋子 则可达
        if (it == board.situation.end() || it->second->color != this->color)
        {
            return true;
        }
        // 在九宫格之内、有棋子且是自己的棋子，则不可达
        return false;
    };
    for (const Pos &d : dir)
    {
        Pos dest = {p.x + d.x, p.y + d.y};
        if (canReach(dest))
        {
            targets.push_back(dest);
        }
    }
    return targets;
}
String Mandarins::getPieceName() { return color == COLOR::RED_WAY ? "仕" : "士"; }

// General
General::~General() {}
VecPos General::Predict(const Board &board) {
    VecPos targets;
    const Pos &p = Position;
    static const VecPos dir = {
        {0, 1}, {0, -1}, {1, 0}, {-1, 0}};
    auto canReach = [&](const Pos &destination) -> bool
    {
        // 目的地不在九宫格内，不可达
        if (!inCamp(this->color, destination))
        {
            return false;
        }
        // 在九宫格内，查成分
        auto it = board.situation.find(destination);
        // 空 / 敌方棋子，可达
        if (it == board.situation.end() || it->second->color != this->color)
        {
            return true;
        }
        // 我方棋子，不可达
        return false;
    };
    for (const Pos &d : dir)
    {
        Pos dest = {p.x + d.x, p.y + d.y};
        if (canReach(dest))
        {
            targets.push_back(dest);
        }
    }
    return targets;
}
String General::getPieceName() { return color == COLOR::RED_WAY ? "帅" : "将"; }

// Broad
Board::Board() : situation(initializer()) {}
PosMap Board::initializer() {
    PosMap posMap;
    // 红方
    posMap[{0, 0}] = std::make_unique<Rooks>(Pos{0, 0}, true);
    posMap[{0, 1}] = std::make_unique<Knights>(Pos{0, 1}, true);
    posMap[{0, 2}] = std::make_unique<Elephants>(Pos{0, 2}, true);
    posMap[{0, 3}] = std::make_unique<Mandarins>(Pos{0, 3}, true);
    posMap[{0, 4}] = std::make_unique<General>(Pos{0, 4}, true);
    posMap[{0, 5}] = std::make_unique<Mandarins>(Pos{0, 5}, true);
    posMap[{0, 6}] = std::make_unique<Elephants>(Pos{0, 6}, true);
    posMap[{0, 7}] = std::make_unique<Knights>(Pos{0, 7}, true);
    posMap[{0, 8}] = std::make_unique<Rooks>(Pos{0, 8}, true);
    posMap[{2, 1}] = std::make_unique<Cannons>(Pos{2, 1}, true);
    posMap[{2, 7}] = std::make_unique<Cannons>(Pos{2, 7}, true);
    posMap[{3, 0}] = std::make_unique<Pawns>(Pos{3, 0}, true);
    posMap[{3, 2}] = std::make_unique<Pawns>(Pos{3, 2}, true);
    posMap[{3, 4}] = std::make_unique<Pawns>(Pos{3, 4}, true);
    posMap[{3, 6}] = std::make_unique<Pawns>(Pos{3, 6}, true);
    posMap[{3, 8}] = std::make_unique<Pawns>(Pos{3, 8}, true);
    // 黑方
    posMap[{9, 0}] = std::make_unique<Rooks>(Pos{9, 0}, false);
    posMap[{9, 1}] = std::make_unique<Knights>(Pos{9, 1}, false);
    posMap[{9, 2}] = std::make_unique<Elephants>(Pos{9, 2}, false);
    posMap[{9, 3}] = std::make_unique<Mandarins>(Pos{9, 3}, false);
    posMap[{9, 4}] = std::make_unique<General>(Pos{9, 4}, false);
    posMap[{9, 5}] = std::make_unique<Mandarins>(Pos{9, 5}, false);
    posMap[{9, 6}] = std::make_unique<Elephants>(Pos{9, 6}, false);
    posMap[{9, 7}] = std::make_unique<Knights>(Pos{9, 7}, false);
    posMap[{9, 8}] = std::make_unique<Rooks>(Pos{9, 8}, false);
    posMap[{7, 1}] = std::make_unique<Cannons>(Pos{7, 1}, false);
    posMap[{7, 7}] = std::make_unique<Cannons>(Pos{7, 7}, false);
    posMap[{6, 0}] = std::make_unique<Pawns>(Pos{6, 0}, false);
    posMap[{6, 2}] = std::make_unique<Pawns>(Pos{6, 2}, false);
    posMap[{6, 4}] = std::make_unique<Pawns>(Pos{6, 4}, false);
    posMap[{6, 6}] = std::make_unique<Pawns>(Pos{6, 6}, false);
    posMap[{6, 8}] = std::make_unique<Pawns>(Pos{6, 8}, false);
    return posMap;
}
bool Board::move(const Move &moveData) {
    // 需要 C++17
    auto go = [&]() {
        // 键值对移动逻辑
        auto transfer = situation.extract(moveData.from);
        transfer.key() = moveData.to;
        situation.insert(std::move(transfer));
    };
    /*  返回 true 表示移动成功
     *  返回 false 表示移动失败
     */
    // From 位置的 PosMap 的 Iterator
    auto fpmi = situation.find(moveData.from);
    // 没这个棋子
    if (fpmi == situation.end()) { return false; }
    // 你拿了别人的棋子
    if (fpmi->second->color != moveData.color) { return false; }

    // to 位置的 PosMap 迭代器
    auto tpmi = situation.find(moveData.to);
    // 注意，设计上 to 位置只能由 predict 得到，所以默认是合法的位置，但需要检查上面有没有棋子
    // 这个是没有棋子的情况，直接移动
    if (tpmi == situation.end()) {
        go();
        return true;
    }
    // 剩下有棋子的情况，经过 predict 的过滤只能是敌方棋子
    // 但还是要判断一下
    if (tpmi->second->color != moveData.color) {
        situation.erase(moveData.to);
        go();
        return true;
    }
    // 防御
    return false;
}

// Player
Player::Player() : name("tmp") {}
Player::Player(String str) : name(str) {}
Player::Player(std::ifstream &initFile) { deserialize(initFile); }

//
void Player::serialize(std::ofstream &sf) {
    /* TODO */
}
void Player::deserialize(std::ifstream &lf) {
    /* TODO */
}

// Move
Move::Move() : from(), to() {}
Move::Move(int compressedData) {
    int& n = compressedData;
    to.y   =  n         % 10;
    to.x   = (n / 10)   % 10;
    from.y = (n / 100)  % 10;
    from.x = (n / 1000) % 10;
}
int Move::compressMove() const {
    // 用一个四位数表示从哪来到哪去
    // 例如"把(1, 4)处的棋子移动到(2,3)"可以写成 1423
    const int& x1 = from.x;
    const int& y1 = from.y;
    const int& x2 = to.x;
    const int& y2 = to.y;
    return x1*1000 + y1*100 + x2*10 + y2;
}
void Move::serialize(std::ofstream &sf) const {
    sf.write(reinterpret_cast<const char *>(&from.x), sizeof(from.x));
    sf.write(reinterpret_cast<const char *>(&from.y), sizeof(from.y));
    sf.write(reinterpret_cast<const char *>(&to.x), sizeof(to.x));
    sf.write(reinterpret_cast<const char *>(&to.y), sizeof(to.y));
    // 移动本身已经包含 color 信息
}
void Move::deserialize(std::ifstream &lf) {
    lf.read(reinterpret_cast<char *>(&from.x), sizeof(from.x));
    lf.read(reinterpret_cast<char *>(&from.y), sizeof(from.y));
    lf.read(reinterpret_cast<char *>(&to.x), sizeof(to.x));
    lf.read(reinterpret_cast<char *>(&to.y), sizeof(to.y));
    // 移动本身已经包含 color 信息
}
std::istream &operator>>(std::istream &is, Move &move)
{
    // 逻辑：读取顺序输入的from.x, from.y, to.x, to.y
    unsigned int n;
    is >> n;
    move.to.y = n % 10;
    n /= 10;
    move.to.x = n % 10;
    n /= 10;
    move.from.y = n % 10;
    n /= 10;
    move.from.x = n % 10;
    return is;
}


// moveRequest
void movRequest::movFromLocal(XiangqiGame& game, Move& mov) {
    // 在移动前插入本地的所属方数据
    mov.color = game.gamePlayerLocal.way;
    game.executeMove(mov);
}
void movRequest::movFromINET(XiangqiGame& game, Move& mov) {
    // 移动前插入对方的所属方数据
    mov.color = game.gamePlayerINET.way;
    game.executeMove(mov);
}

// XiangqiGame
XiangqiGame::XiangqiGame() : gamePlayerLocal(), gamePlayerINET(), gameBoard() {}
XiangqiGame::XiangqiGame(const Player &p1, const Player &p2) : gamePlayerLocal(p1), gamePlayerINET(p2), gameBoard() {}

void XiangqiGame::saveHistory(const String& fileName) {
    // code
}
void XiangqiGame::loadHistory(const String& fileName) {
    // code
}

Move XiangqiGame::getPlayerMove(bool currPlayer) {
    Move moveData;
    moveData.color = currPlayer;
    // 从控制台输入from的坐标和to的坐标，中间不要有空格
    // 例如输入：4213
    Move getMove;
    std::cin >> getMove;
    return moveData;
}
bool XiangqiGame::isValid(const Move &moveData) {
    // 获取棋盘状态
    const PosMap &situ = gameBoard.situation;
    // 检查棋子
    auto c_it = situ.find(moveData.from);
    if (c_it == situ.end()) { return false; } // 这没棋子

    // 获取所有可达地点；轮询检查目标是否可达
    const VecPos &poses = c_it->second->Predict(gameBoard);
    for (auto p : poses)
    {
        if (p == moveData.to) { return true; }
    }
    return false;
}
void XiangqiGame::executeMove(const Move &moveData) {
    gameBoard.move(moveData);
    history.push(moveData); // 记录移动历史
}
bool XiangqiGame::checkMate(const Move &lastMove) {
    // TODO
    // 还没想好
    return gameFinish;
}

void XiangqiGame::GamePlay() {
    const PosMap &boardMap = gameBoard.situation;
    // 关于输入
    // 本质上都是要么从文件读取，要么从命令行读取，要么从网络通信读取
    // 封装一个Move对象用于构造输入，再Game类中兼容不同的对象

    /* 梳理一下逻辑吧
     *  首先需要构造 Move 对象接收 移动信息(from to)
     *  在 isValid 函数中使用判断移动信息
     *  使用 executeMove 执行移动
     *  然后根据整个棋盘的信息，在 checkMate 函数中判断是否将军或者和棋之类
     *
     *  所以整个这一段就是在综合处理移动，也就是象棋的核心玩法：移动棋子
     *  起名为 GamePlay 不为过
     */

    // 似乎不需要这个循环，只需要在主循环里每次调用即可
    // TODO 需要重新思考一下游戏流程
    Move inputMoveData = getPlayerMove(nowTurn);
    if (isValid(inputMoveData))
    {
        executeMove(inputMoveData);
        checkMate(inputMoveData);
        nowTurn = !nowTurn;
    }
}

const Board &XiangqiGame::GetBoardInfo() const {
    const Board &situation = gameBoard;
    return situation;
}
