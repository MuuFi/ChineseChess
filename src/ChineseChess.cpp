#include "ChineseChess.h"

using Pos = std::pair<int, int>;
using VecXQ = std::vector<XiangqiPiece>;
using String = std::string;

XiangqiPiece::XiangqiPiece(char ch = 0): type(ch), Position(0, 0) {}
XiangqiPiece::XiangqiPiece(char ch, const Pos& p): type(ch), Position(p) {}

Player::Player(): name(), retainPieces() {}
Player::Player(String str): name(str), retainPieces() {}
Player::Player(String str, const VecXQ& initialize): name(str), retainPieces(initialize) {}
Player::Player(std::ifstream& initFile) { deserialize(initFile); }

// TODO: 一堆函数没写
void Player::serialize(std::ofstream& sf) {}
void Player::deserialize(std::ifstream& lf) {}
std::vector<Pos> Player::predicted(size_t PieceIdx) {}
void Player::move(size_t PieceIdx, const Pos& targetPos) {}
