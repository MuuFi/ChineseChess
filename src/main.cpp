#include "Chess_i.h"

int main() {

    // 本地通信测试，互发 Msg 代码段
    /*
    int db;
    std::cin >> db;
    char buf[1024] = {0};
    String dbMsg = String(buf, 1024);

    if(db == 1) {
        Server serv;
        serv.startRecvThread();
        while (true) {
            String msg = std::move(serv.getLastMsg());
            if (msg.size()) { std::cout << "[recv]:" << msg << std::endl; }
            std::getline(std::cin, dbMsg);
            serv.sendMsg(dbMsg);
            if (!serv.isConnected()) { break; }
            dbMsg.clear();
        }
    }
    else if(db == 2) {
        Client clie;
        clie.startRecvThread();
        while (true) {
            
            String msg = std::move(clie.getLastMsg());
            if(msg.size()) { std::cout << "[recv]:" << msg << std::endl; }
            std::getline(std::cin, dbMsg);
            clie.sendMsg(dbMsg);
            if (!clie.isConnected()) { break; }
            dbMsg.clear();
        }

    }
    */

    // 图形显示测试
    
    XiangqiGame Game;
    ChessGUI gui(Game.GetBoardInfo());
    while (!gui.ExitGame()) {
        // 主循环
        gui.clickCheck();
        gui.renderUpdated();
    }
    
    return 0;
}