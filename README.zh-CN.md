# 中国象棋 - C++

为什么要重复造轮子？<br>
~~因为这是对我的锻炼，我可以借此学习很多东西<br>~~
因为我想<br>

第一次写README，不符合规范请指出<br>


## 介绍

### 项目进度
- [ ] 计划用C++实现中国象棋的棋盘、棋子、规则等逻辑：进行中
- [ ] 计划通过内置HTTP服务对外提供接口，让其他用户可以通过网络访问本地服务进行对弈：暂未开始



### 当前已有的
 - 基本的棋盘和棋子数据结构：完善中
 - 棋子走法规则实现
 - CMake 构建支持
 - 代码写的还不错吧我猜（我没有认知）



### 编译方法
**项目使用~~我现学现卖的~~CMake构建，目前暂未测试最低的C++版本但C++20肯定没什么问题（大概）**<br>
~~我没记错的话应该没有用C++14以上的特性~~
```bash
git clone ${TODO: add url}
cd ChineseChess
mkdir build
cd build
cmake ..
cmake --build .
```



### 运行方法
目前是直接运行可执行文件<br>
```bash
# 很抱歉可执行程序的名字和项目名字不一样实在是需要的话我会找个时间改
./ChessGame
```
~~~~
请注意，没有user interface
虽然会出现控制台界面但是没有办法直接进行交互
因为还在开发阶段
~~~~

### 目录结构
```
ChineseChess\
├── include\           # 头文件
│   └── ChineseChess.h
├── src\               # 源文件
│   ├── ChineseChess.cpp
│   └── main.cpp
├── CMakeLists.txt     # CMake 构建脚本
├── README.md
└── README.zh-CN.md
```


### 未来规划

……先把基础功能写完吧