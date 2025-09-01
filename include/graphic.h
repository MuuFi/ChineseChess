#ifndef GRAPHIC_H
#define GRAPHIC_H

#define RAYLIB_WINDOWS_NO_WIN32_DEFINES
#include "raylib.h"
#include "ChineseChess.h"
#include <math.h>

class ChessUI {
private:
    const Board* observer;
    bool mouseDown;

public:
    ChessUI(const Board* board_ptr);
    ~ChessUI();
    void clickCheck();

};
ChessUI::ChessUI(const Board* board_ptr): observer(board_ptr) {
    /* code */
}
ChessUI::~ChessUI() {}
// 检查点击事件 ING
void ChessUI::clickCheck() {
    static Vector2 clickPos;
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        mouseDown = true;
        TraceLog(LOG_INFO, "press detected");
    }
    else if(mouseDown && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        mouseDown = false;
        clickPos = GetMousePosition();
        TraceLog(LOG_INFO, "click checked at %.0f, %.0f", clickPos.x, clickPos.y);
    }
}

class ChessGUI
{
private:
    /* data */
    int Width = 800;
    int Height = 600;
    ChessUI ui;

    void initGraphic();
    void DrawChineseChessBoard(int X, int Y, int cellW, int cellH, int cols, int rows);
    void DrawPieces(int posX, int posY);
    ChessGUI(const ChessGUI&) = delete;
    ChessGUI operator=(const ChessGUI&) = delete;
public:
    ChessGUI(const Board* board_ptr);
    ~ChessGUI();
    void clickCheckHandler();
    void renderUpdated();
    void renderPieces(const Board& situation, bool isFontLoaded);
};


// 绘制初始化
void ChessGUI::initGraphic() {
    InitWindow(Width, Height, "ChineseChess");
    SetTargetFPS(60);
    // 开始绘制流程
    renderUpdated();
    // TODO 搞定字体文件的加载

}
// 画个棋盘，需要精简化
inline void ChessGUI::DrawChineseChessBoard(int X, int Y, int cellW, int cellH, int cols, int rows) {
    
    // 绘制横线
    for (int i = 0; i < rows; ++i) {
        DrawLineEx(
            { (float)X, (float)(Y + i * cellH) },
            { (float)(X + (cols - 1) * cellW), (float)(Y + i * cellH) },
            1.0f, BLACK
        );
    }

    // 绘制纵线
    for (int i = 0; i < cols; ++i) {
        if(i == 0 || i == cols -1) {
            DrawLineEx(
                { (float)(X + i * cellW), (float)Y },
                { (float)(X + i * cellW), (float)(Y + 8 * cellH) },
                1.0f, BLACK
            );
        }
        // 上半部分
        DrawLineEx(
            { (float)(X + i * cellW), (float)Y },
            { (float)(X + i * cellW), (float)(Y + 4 * cellH) },
            1.0f, BLACK
        );
        // 下半部分
        DrawLineEx(
            { (float)(X + i * cellW), (float)(Y + 5 * cellH) },
            { (float)(X + i * cellW), (float)(Y + 9 * cellH) },
            1.0f, BLACK
        );
    }

    // 绘制楚河汉界
    DrawText("ChuRiver", X + cellW + 1, Y + 4 * cellH + 5, 24, DARKGRAY);
    DrawText("HanBorow", X + 5 * cellW + 5, Y + 4 * cellH + 5, 24, DARKGRAY);

    // 绘制将/帅九宫格斜线
    DrawLineEx(
        { (float)(X + 3 * cellW), (float)Y },
        { (float)(X + 5 * cellW), (float)(Y + 2 * cellH) },
        1.4f, RED
    );
    DrawLineEx(
        { (float)(X + 5 * cellW), (float)Y },
        { (float)(X + 3 * cellW), (float)(Y + 2 * cellH) },
        1.4f, RED
    );
    DrawLineEx(
        { (float)(X + 3 * cellW), (float)(Y + 7 * cellH) },
        { (float)(X + 5 * cellW), (float)(Y + 9 * cellH) },
        1.4f, RED
    );
    DrawLineEx(
        { (float)(X + 5 * cellW), (float)(Y + 7 * cellH) },
        { (float)(X + 3 * cellW), (float)(Y + 9 * cellH) },
        1.4f, RED
    );

    // 绘制边沿线
    DrawLineEx(
        { (float)(X - 4), (float)(Y - 4) },
        { (float)(X + 8 * cellW + 4), (float)(Y - 4) },
        2.0f, BLACK
    );
    DrawLineEx(
        { (float)(X + 8 * cellW + 4), (float)(Y - 4) },
        { (float)(X + 8 * cellW + 4), (float)(Y + 9 * cellH + 4) },
        2.0f, BLACK
    );
    DrawLineEx(
        { (float)(X + 8 * cellW + 4), (float)(Y + 9 * cellH + 4) },
        { (float)(X - 4), (float)(Y + 9 * cellH + 4) },
        2.0f, BLACK
    );
    DrawLineEx(
        { (float)(X - 4), (float)(Y + 9 * cellH + 4) },
        { (float)(X - 4), (float)(Y - 4) },
        2.0f, BLACK
    );
}
// 画个棋子
inline void ChessGUI::DrawPieces(int posX, int posY) {
    /* 需要棋子的绘制代码 */
    
}
// 构造/析构
ChessGUI::ChessGUI(const Board* board_ptr): ui(ChessUI(board_ptr)) {
    initGraphic();
    SetWindowMinSize(800, 600);
    SetWindowMaxSize(2560, 1440);
}
ChessGUI::~ChessGUI() {
    CloseWindow();
}
// 检查点击事件
void ChessGUI::clickCheckHandler() {
    ui.clickCheck();
}
// 渲染背景
void ChessGUI::renderUpdated() {
    // 开始绘图
    BeginDrawing();
    ClearBackground(RAYWHITE);// 设置背景颜色

    // 棋盘参数
    const int startX = 50;
    const int startY = 75;
    const int cellW = 50;
    const int cellH = 50;
    const int cols = 9;
    const int rows = 10;
    DrawChineseChessBoard(startX, startY, cellW, cellH, cols, rows);

    // 分割线
    DrawLineEx({500, 0}, {500, 600}, 2.0f, BLUE);
    DrawLineEx({503, 0}, {503, 600}, 2.0f, BLUE);

    // 暂时不做，聊天界面
    /*code*/

    // 显示 FPS
    DrawFPS(10, 10);
    // 结束绘图
    EndDrawing();
}
// 渲染棋子
void ChessGUI::renderPieces(const Board& board, bool isFontLoaded) {
    // 参数
    const int startX = 50;
    const int startY = 75;
    const int cellW = 50;
    const int cellH = 50;
    for(auto it = board.situation.begin(); it != board.situation.end(); it++) {
        const Pos& p = it->first;
        const auto& pieces = it->second;

        int X = startX + p.x*cellW;
        int Y = startY + (8 - p.y)*cellH;

        DrawPieces(X, Y);
    }
}
#endif