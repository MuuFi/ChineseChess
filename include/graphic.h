#ifndef GRAPHIC_H
#define GRAPHIC_H

#include "raylib.h"
class ChessGUI
{
private:
    /* data */
    void initGraphic();
    ChessGUI(const ChessGUI&) = delete;
    ChessGUI operator=(const ChessGUI&) = delete;
public:
    ChessGUI();
    ~ChessGUI();
};

inline void ChessGUI::initGraphic() {
    InitWindow(800, 600, "ChineseChess");
    SetTargetFPS(60);

    BeginDrawing();// 开始绘图
    // 绘图代码
    ClearBackground(RAYWHITE);



    EndDrawing();// 结束绘图
}
inline ChessGUI::ChessGUI() {
    initGraphic();
}
ChessGUI::~ChessGUI() {
    CloseWindow();
}

#endif