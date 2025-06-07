#ifndef CHINESECHESS_H
#define CHINESECHESS_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <utility>

struct XiangqiPiece {
    char type;
    std::pair<int, int> Position;
    XiangqiPiece(char ch);
};
class Player {
private:
    std::string name;
    std::vector<XiangqiPiece> retainPieces;
public:
    Player();
    Player(std::string);
    Player(std::string, const std::vector<XiangqiPiece>& initialize);
    Player(std::ifstream& initFile);

    void serialize();
};
class XiangqiGame {
private:
    Player gamePlayer[2];
public:

};
#endif