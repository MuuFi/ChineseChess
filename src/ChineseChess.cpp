#include "ChineseChess.h"

// using
using Pos = std::pair<int, int>;
using VecXQ = std::vector<XiangqiPiece>;
using String = std::string;
using TypeMap = std::unordered_map<char, std::string>;

XiangqiPiece::XiangqiPiece(char ch = 0): type(ch), Position(0, 0) {}
XiangqiPiece::XiangqiPiece(char ch, const Pos& p): type(ch), Position(p) {}


Player::Player(): name("tmp") {}
Player::Player(String str): name(str) {}
Player::Player(std::ifstream& initFile) { deserialize(initFile); }

// TODO: 一堆函数没写
void Player::serialize(std::ofstream& sf) {}
void Player::deserialize(std::ifstream& lf) {}

const VecXQ XiangqiGame::apInitData = {
    // TODO: 添加棋子数据
};
const TypeMap XiangqiGame::typeName = {
    // TODO: 添加char→string的关系
};
XiangqiGame::XiangqiGame(): gamePlayer(), allPieces(apInitData) {}
XiangqiGame::XiangqiGame(const Player& p1, const Player& p2): gamePlayer(p1, p2), allPieces(apInitData) {}

// TODO: 一堆函数没写
std::vector<Pos> XiangqiGame::predicted(size_t pieceIdx) const {
    return std::vector<Pos>();
}
void XiangqiGame::move(size_t pieceIdx, const Pos& target) {
    if(pieceIdx >= 0 && pieceIdx < allPieces.size()) {
        // debug 记录
        Pos debug_record = allPieces[pieceIdx].Position;
        // 移动逻辑
        allPieces[pieceIdx].Position = target;
        // debug 输出
        std::cout
        << allPieces[pieceIdx].type 
        << " at " 
        << debug_record.first << debug_record.second
        << " has been moved to position: "
        << allPieces[pieceIdx].Position.first << allPieces[pieceIdx].Position.second
        << std::endl;
    } else {
        std::cout << "index out of range" << std::endl;
    }
}