#include <thread>
#include <atomic>
#include "Chess_i.h"

std::atomic<bool> running(true);
void recvThreadClient(Client* cli) {
    while(running) {
        String msg = cli->recvMsg();
        if (msg.size()) {
            std::cout << "[recv]:" << msg << std::endl;
            if (msg.at(0) == 'q') {
                running = false;
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
int main() {
    // need a board in XiangqiGame to init gui

    // 在主循环维护数据的发送和接收
    // XiangqiGame Game;
    
    // ChessGUI gui(&Game.GetBoardInfo());


    int db;
    std::cin >> db;
    char buf[1024] = {0};
    String dbMsg = String(buf, 1024);

    if(db == 1) {
        Server serv;
        serv.startRecvThread();
        while (running) {
            std::getline(std::cin, dbMsg);
            serv.sendMsg(dbMsg);
            if()
            dbMsg.clear();
        }
    }
    else if(db == 2) {
        Client   cli;
        std::thread cli_t(recvThreadClient, &cli);
        while (true) {
            std::getline(std::cin, dbMsg);
            cli.sendMsg(dbMsg);
            if(dbMsg.size() > 0 && dbMsg.at(0) == 'q') {
                running = false;
                break;
            }
            dbMsg.clear();
        }
        cli_t.join();
    }
    
    /*
    while (!gui.ExitGame()) {
        // 主循环
        gui.clickCheckHandler();
        gui.renderUpdated();
    }
    */
    return 0;
}