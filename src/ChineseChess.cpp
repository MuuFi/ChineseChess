#include "ChineseChess.h"

// using
using Pos = std::pair<int, int>;
using VecXQ = std::vector<XiangqiPiece>;
using String = std::string;
using TypeMap = std::unordered_map<char, std::string>;

XiangqiPiece::XiangqiPiece(String str, const Pos& p, bool isRed): type(str), Position(p), color(isRed) {}


Player::Player(): name("tmp") {}
Player::Player(String str): name(str) {}
Player::Player(std::ifstream& initFile) { deserialize(initFile); }

// TODO: 一堆函数没写
void Player::serialize(std::ofstream& sf) {}
void Player::deserialize(std::ifstream& lf) {}

using XP = XiangqiPiece;
enum COLOR { RED = true, BLACK = false };
const VecXQ XiangqiGame::apInitData = {
    // 红方
    XP("车", {0, 0}, COLOR::RED),
    XP("马", {1, 0}, COLOR::RED),
    XP("相", {2, 0}, COLOR::RED),
    XP("仕", {3, 0}, COLOR::RED),
    XP("帅", {4, 0}, COLOR::RED),
    XP("仕", {5, 0}, COLOR::RED),
    XP("相", {6, 0}, COLOR::RED),
    XP("马", {7, 0}, COLOR::RED),
    XP("车", {8, 0}, COLOR::RED),
    XP("炮", {1, 2}, COLOR::RED),
    XP("炮", {7, 2}, COLOR::RED),
    XP("兵", {0, 3}, COLOR::RED),
    XP("兵", {2, 3}, COLOR::RED),
    XP("兵", {4, 3}, COLOR::RED),
    XP("兵", {6, 3}, COLOR::RED),
    XP("兵", {8, 3}, COLOR::RED),
    // 黑方
    XP("车", {0, 9}, COLOR::BLACK),
    XP("马", {1, 9}, COLOR::BLACK),
    XP("象", {2, 9}, COLOR::BLACK),
    XP("士", {3, 9}, COLOR::BLACK),
    XP("将", {4, 9}, COLOR::BLACK),
    XP("士", {5, 9}, COLOR::BLACK),
    XP("象", {6, 9}, COLOR::BLACK),
    XP("马", {7, 9}, COLOR::BLACK),
    XP("车", {8, 9}, COLOR::BLACK),
    XP("炮", {1, 7}, COLOR::BLACK),
    XP("炮", {7, 7}, COLOR::BLACK),
    XP("卒", {0, 6}, COLOR::BLACK),
    XP("卒", {2, 6}, COLOR::BLACK),
    XP("卒", {4, 6}, COLOR::BLACK),
    XP("卒", {6, 6}, COLOR::BLACK),
    XP("卒", {8, 6}, COLOR::BLACK)
};
const bool XiangqiGame::peInitData[9][10] = {
    // x=0~8, y=0~9
    // x=0
    {1,0,0,1,0,0,1,0,0,1},
    // x=1
    {1,0,0,0,0,0,0,1,0,1},
    // x=2
    {1,0,0,1,0,0,1,0,0,1},
    // x=3
    {1,0,0,0,0,0,0,0,0,1},
    // x=4
    {1,0,0,1,0,0,1,0,0,1},
    // x=5
    {1,0,0,0,0,0,0,1,0,1},
    // x=6
    {1,0,0,1,0,0,1,0,0,1},
    // x=7
    {1,0,0,0,0,0,0,1,0,1},
    // x=8
    {1,0,0,1,0,0,1,0,0,1}
};
XiangqiGame::XiangqiGame() : gamePlayer(), allPieces(apInitData) {
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 10; ++j)
            pieceExist[i][j] = peInitData[i][j];
}
XiangqiGame::XiangqiGame(const Player& p1, const Player& p2)
    : gamePlayer{p1, p2}, allPieces(apInitData) {
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 10; ++j)
            pieceExist[i][j] = peInitData[i][j];
}

std::vector<Pos> XiangqiGame::predicted(size_t pieceIdx) const {
    const XiangqiPiece& piece = allPieces[pieceIdx];
    const String& t = piece.type;
    std::vector<Pos> moves;
    int x = piece.Position.first;
    int y = piece.Position.second;
    bool isRed = piece.color;

    auto inBoard = [](int x, int y) {
        return x >= 0 && x <= 8 && y >= 0 && y <= 9;
    };

    // 兵/卒
    if (t == "兵" || t == "卒") {
        int dir = isRed ? 1 : -1;
        int ny = y + dir;
        if (inBoard(x, ny) && (!pieceExist[x][ny] || allPieces[pieceIdx].color != pieceExist[x][ny])) moves.push_back({x, ny});
        if ((isRed && y >= 5) || (!isRed && y <= 4)) {
            if (inBoard(x - 1, y) && (!pieceExist[x - 1][y] || allPieces[pieceIdx].color != pieceExist[x - 1][y])) moves.push_back({x - 1, y});
            if (inBoard(x + 1, y) && (!pieceExist[x + 1][y] || allPieces[pieceIdx].color != pieceExist[x + 1][y])) moves.push_back({x + 1, y});
        }
    }
    // 车
    else if (t == "车") {
        // 向右
        for (int nx = x + 1; nx <= 8; ++nx) {
            if (pieceExist[nx][y]) { moves.push_back({nx, y}); break; }
            moves.push_back({nx, y});
        }
        // 向左
        for (int nx = x - 1; nx >= 0; --nx) {
            if (pieceExist[nx][y]) { moves.push_back({nx, y}); break; }
            moves.push_back({nx, y});
        }
        // 向下
        for (int ny = y + 1; ny <= 9; ++ny) {
            if (pieceExist[x][ny]) { moves.push_back({x, ny}); break; }
            moves.push_back({x, ny});
        }
        // 向上
        for (int ny = y - 1; ny >= 0; --ny) {
            if (pieceExist[x][ny]) { moves.push_back({x, ny}); break; }
            moves.push_back({x, ny});
        }
    }
    // 马
    else if (t == "马") {
        // 马脚判定
        const int dx[] = {1, 2, 2, 1, -1, -2, -2, -1};
        const int dy[] = {2, 1, -1, -2, -2, -1, 1, 2};
        const int legx[] = {0, 1, 1, 0, 0, -1, -1, 0};
        const int legy[] = {1, 0, 0, -1, -1, 0, 0, 1};
        for (int i = 0; i < 8; ++i) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            int lx = x + legx[i];
            int ly = y + legy[i];
            if (inBoard(nx, ny) && !pieceExist[lx][ly]) {
                if (!pieceExist[nx][ny] || allPieces[pieceIdx].color != pieceExist[nx][ny])
                    moves.push_back({nx, ny});
            }
        }
    }
    // 炮
    else if (t == "炮") {
        // 横向
        for (int nx = x + 1, jumped = 0; nx <= 8; ++nx) {
            if (!jumped) {
                if (pieceExist[nx][y]) jumped = 1;
                else moves.push_back({nx, y});
            } else {
                if (pieceExist[nx][y]) { moves.push_back({nx, y}); break; }
            }
        }
        for (int nx = x - 1, jumped = 0; nx >= 0; --nx) {
            if (!jumped) {
                if (pieceExist[nx][y]) jumped = 1;
                else moves.push_back({nx, y});
            } else {
                if (pieceExist[nx][y]) { moves.push_back({nx, y}); break; }
            }
        }
        // 纵向
        for (int ny = y + 1, jumped = 0; ny <= 9; ++ny) {
            if (!jumped) {
                if (pieceExist[x][ny]) jumped = 1;
                else moves.push_back({x, ny});
            } else {
                if (pieceExist[x][ny]) { moves.push_back({x, ny}); break; }
            }
        }
        for (int ny = y - 1, jumped = 0; ny >= 0; --ny) {
            if (!jumped) {
                if (pieceExist[x][ny]) jumped = 1;
                else moves.push_back({x, ny});
            } else {
                if (pieceExist[x][ny]) { moves.push_back({x, ny}); break; }
            }
        }
    }
    // 相/象
    else if (t == "相" || t == "象") {
        const int dx[] = {2, 2, -2, -2};
        const int dy[] = {2, -2, 2, -2};
        const int legx[] = {1, 1, -1, -1};
        const int legy[] = {1, -1, 1, -1};
        for (int i = 0; i < 4; ++i) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            int lx = x + legx[i];
            int ly = y + legy[i];
            if (inBoard(nx, ny) && !pieceExist[lx][ly]) {
                if ((isRed && ny <= 4) || (!isRed && ny >= 5))
                    if (!pieceExist[nx][ny] || allPieces[pieceIdx].color != pieceExist[nx][ny])
                        moves.push_back({nx, ny});
            }
        }
    }
    // 仕/士
    else if (t == "仕" || t == "士") {
        const int dx[] = {1, 1, -1, -1};
        const int dy[] = {1, -1, 1, -1};
        for (int i = 0; i < 4; ++i) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            if (inBoard(nx, ny) && nx >= 3 && nx <= 5) {
                if ((isRed && ny >= 0 && ny <= 2) || (!isRed && ny >= 7 && ny <= 9))
                    if (!pieceExist[nx][ny] || allPieces[pieceIdx].color != pieceExist[nx][ny])
                        moves.push_back({nx, ny});
            }
        }
    }
    // 帅/将
    else if (t == "帅" || t == "将") {
        const int dx[] = {1, -1, 0, 0};
        const int dy[] = {0, 0, 1, -1};
        for (int i = 0; i < 4; ++i) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            if (inBoard(nx, ny) && nx >= 3 && nx <= 5) {
                if ((isRed && ny >= 0 && ny <= 2) || (!isRed && ny >= 7 && ny <= 9))
                    if (!pieceExist[nx][ny] || allPieces[pieceIdx].color != pieceExist[nx][ny])
                        moves.push_back({nx, ny});
            }
        }
    }
    return moves;
}
void XiangqiGame::move(size_t pieceIdx, const Pos& target) {
    if(pieceIdx >= 0 && pieceIdx < allPieces.size()) {
        Pos oldPos = allPieces[pieceIdx].Position;
        pieceExist[oldPos.first][oldPos.second] = false;
        pieceExist[target.first][target.second] = true;
        allPieces[pieceIdx].Position = target;
        std::cout << allPieces[pieceIdx].type 
                  << " at " << oldPos.first << oldPos.second
                  << " has been moved to position: "
                  << target.first << target.second
                  << std::endl;
    } else {
        std::cout << "index out of range" << std::endl;
    }
}