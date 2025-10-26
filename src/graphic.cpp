#include "raylib.h"
#include <math.h>
#include "ChineseChess.h"
#include "Chess_i.h"



/* other Data */
static const size_t TotalCharactorInGame = 15; // 我数了，一共 15 个字

/* ChessGUI[private] */
// impl 结构体定义
struct ChessGUI::ChessGUI_Impl {
    int     Width;
    int     Height;
    Font    ChessFont;
    std::vector<uint32_t> codePoints;
    const Board& observer;


    ChessGUI_Impl(const Board& ob): Width(800), Height(600), observer(ob) {}
};



// 画棋盘
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

    /*
    由于实际是底层api调用，所以字体文件必须在openGL上下文加载之后加载
    注意ttf文件和伪装成ttf的ttc文件，只有ttf能被正常解析（考虑ttc转换成ttf的方式）
    我必须为此写md，这是第二次遇到这个问题，上次是误打误撞搞好的，或者搞好了想通了但是忘了，写笔记是第一优先级
    */
    // 绘制楚河汉界
    // DrawText("测试", 100, 50, 32, RED);
    // 下面这句可以正常渲染说明 raylib 可能自带 utf8解析，但我不认为我的学习是没意义的
    // DrawTextEx(pimpl->ChessFont, "楚河", {100, 150}, 32.0f, 2.0f, RED);
    // DrawTextCodepoint(pimpl->ChessFont, 0x695A, {100, 200}, 32.0f, RED);
    // DrawTextCodepoints(pimpl->ChessFont, (int*)(pimpl->codePoints.data()), 2, {100, 250}, 32.0f, 2.0f, RED);

    renderPieces(pimpl->observer);
    // 还要封装一层视图，根据当前玩家方颠倒视图

    // 绘制将/帅九宫格斜线
    {
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
    }
    // 绘制边沿线
    {
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
}
// 画棋子
inline void ChessGUI::DrawPieces(int posX, int posY, Pieces* p) {
    float r1 = 20.0f;
    float r2 = 15.0f;
    float cX = posX;
    float cY = posY;
    float tX = posX - r2;
    float tY = posY - r2;
    Color tColor;
    p->color ? tColor = RED : tColor = BLACK;
    DrawRing({cX, cY}, r1-1, r1+1, 0, 360, 100, DARKBLUE);
    // 存在无法画在中心点的问题，这是因为字体渲染以左上角为基准，存在偏置
    DrawTextEx(pimpl->ChessFont, p->getPieceName().c_str(), { tX, tY }, 30.0f, 0.0f, tColor);
    // code
    // TODO0
    // 先画个蓝色的圆形
    // 再把对应颜色的棋子字体画进去

    // 关于贴图大小和棋子大小动态调整的问题可能会是一个棘手的问题
    // 此外可能需要设计一下棋盘，此后可能需要引入棋盘的纹理等
    // 因此在DrawText函数中的几乎所有参数都是需要作为关于屏幕长宽的函数的
}
// UTF8解析
std::vector<uint32_t> ChessGUI::parseUTF8(const String& str) {
    std::vector<uint32_t> codepoints;
    for (size_t i = 0; i < str.size(); ) {
        uint32_t cp = 0;
        unsigned char c = str[i];
        // 学习ING，暂且用二进制
        if (c < 0b10000000) { // 这是ASCII码的范畴
            cp = c;
            i += 1;
        }
        else if ((c >> 5) == 0b110) { // 如果前三位是 110，说明是双字节
            cp = ((c & 0b00011111) << 6) | (str[i+1] & 0b00111111);
            i += 2;
        }
        else if ((c >> 4) == 0b1110) { // 前四位是 1110，说明是三字节
            cp = ((c & 0b00001111) << 12) | ((str[i+1] & 0b00111111) << 6) | (str[i+2] & 0b00111111);
            i += 3;
        }
        else if ((c >> 3) == 0b11110) { // 前五位是 11110，说明是四字节
            cp = ((c & 0b00000111) << 18) | ((str[i+1] & 0b00111111) << 12) | ((str[i+2] & 0b00111111) << 6) | (str[i+3] & 0b00111111);
            i += 4;
        }
        codepoints.push_back(cp);
    }
    return codepoints;
}
/* ChessGUI[public] */
// 构造/析构
ChessGUI::ChessGUI(const Board& ob) {
    pimpl = new ChessGUI_Impl(ob);

    InitWindow(pimpl->Width, pimpl->Height, "ChineseChess");SetTargetFPS(60);

    // 字体文件加载
    String allCharactors = "兵卒車马炮相象士仕将帅楚河汉界";
    pimpl->codePoints = parseUTF8(allCharactors);

    for (uint32_t i : pimpl->codePoints) { TraceLog(LOG_INFO, "%d", i); }
    TraceLog(LOG_INFO, "Working dir: %s", GetWorkingDirectory());

    pimpl->ChessFont = LoadFontEx(
        "resources/LXGWWenKaiMono-Medium.ttf",
        32,
        reinterpret_cast<int*>(pimpl->codePoints.data()),
        pimpl->codePoints.size()
    );
    if (pimpl->ChessFont.texture.id == 0) { TraceLog(LOG_ERROR, "Font load failed!"); }
    else { TraceLog(LOG_INFO, "Font loaded successfully."); }

    // 开始绘制流程
    renderUpdated();
}
ChessGUI::~ChessGUI() {
    UnloadFont(pimpl->ChessFont);
    CloseWindow();
    delete pimpl;
}
// 每帧渲染更新
void ChessGUI::renderUpdated() {
    // 开始绘图
    BeginDrawing();
    ClearBackground(RAYWHITE);// 设置背景颜色

    // 棋盘参数
    const int startX = 50;
    const int startY = 75;
    const int cellW = 50;
    const int cellH = 50;
    const static int cols = 9;
    const static int rows = 10;
    // TODO0 计算参数
    DrawChineseChessBoard(startX, startY, cellW, cellH, cols, rows);

    // 分割线
    // DrawLineEx({500, 0}, {500, 600}, 2.0f, BLUE);
    // DrawLineEx({503, 0}, {503, 600}, 2.0f, BLUE);

    // 聊天
    // code

    // 显示 FPS
    DrawFPS(10, 10);
    // 结束绘图
    EndDrawing();
}
// 渲染棋子
void ChessGUI::renderPieces(const Board& board) {
    // 参数
    const int startX = 50;
    const int startY = 75;
    const int cellW = 50;
    const int cellH = 50;

    for(auto it = board.situation.begin(); it != board.situation.end(); it++) {
        const Pos& p = it->first;
        const auto& pieces = it->second;

        int X = startX + p.y * cellW;
        int Y = startY + (9 - p.x) * cellH;

        DrawPieces(X, Y, pieces.get());
    }
}
// exit
bool ChessGUI::ExitGame() { return WindowShouldClose(); }


// 检查点击事件
void ChessGUI::clickCheck() {
    static Vector2 clickPos;
    static bool mouseDown;
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        mouseDown = true;
        TraceLog(LOG_INFO, "press detected");
    }
    else if (mouseDown && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        mouseDown = false;
        clickPos = GetMousePosition();
        TraceLog(LOG_INFO, "click checked at %.0f, %.0f", clickPos.x, clickPos.y);
    }
}