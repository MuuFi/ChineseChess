#include "graphic.h"

int main() {
    // need a board in XiangqiGame to init gui

    // 在主循环维护数据的发送和接收
    XiangqiGame Game;
    
    ChessGUI gui(&Game.GetBoardInfo());
    while (!WindowShouldClose()) {
        // 主循环
        gui.clickCheckHandler();
        gui.renderUpdated();
    }
    
    return 0;
}