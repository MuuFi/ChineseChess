# 经验汇总
## CMake工作流程
### 头文件（.h）、源文件（.cpp）、可执行文件（.exe等）
content
## socket网络编程
### socket的工作方式
content
### 如何debug
content



## impl模式
### 啥玩意
impl模式（Implementation模式）就是：
```cpp
// a_Class.h
// other code ...
class a_Class {
public:
    // something
private:
    struct impl;
    impl* pimpl;
    // basically there are only these two
};
```
然后
```cpp
// a_Class.cpp
// other code ...
struct a_Class::impl {
    int a;
    std::string b;
    // and other define...
};
```
然后当你需要使用`a`,`b`的时候，通过`pimpl->a/b`访问

这就像脱裤子放屁一样，如果是小项目，头文件之间不会冲突的话，是的这就是脱裤子放屁

### 何时使用？

这就不得不解释什么时候我们需要脱裤子放屁。它主要是为了解决：
**头文件相互之间冲突，导致无法在同一个文件中同时include这两个头文件的情况。**
例如在这个项目中

~~虽然这不是什么大项目但是我摇骰子摇到了冲突的~~

`raylib`库和Windows网络编程需求的库有一部分冲突，所以倘若在头文件中包含这些库，最后到相互协作的时候，他们一旦凑在一起，就无法通过链接

所以为了不让他们凑在一起，我们不能把他们包含进任何头文件里，但是头文件是写类定义的地方，如果不包含，怎么写定义呢？

最简单的答案：impl

我只在最外层留个没写完的struct，成员变量全部放cpp进去，你在cpp里include一下又不会有什么事。什么，又冲突了？那再分一个类出来用impl不就好了

### 优缺点

这个方法几乎不需要脑子，只要稍微了解一下，回去可以直接写出来1000个impl。了解过的话，看得人也能看懂1000个impl

我可以说，每个impl都是脱裤子放屁，但是他脱有脱的道理。不是每个人都喜欢去大肠
~~代码实现~~
里修复自己项目的腹泻，为了不拉在裤子上，拖一下裤子是最简单的方法

因此，impl的优点之一就是：**简单易懂**

同时，这也带来了缺点之一：**访问开销**

如果是在做嵌入式，或者优化热点代码，综合考虑优化难度和可以达成的效果，或许你可以考虑把impl模式优化掉。但是在开发时，为了防止老板把你优化掉，这点性能开销还是不值得作为降低聚合的阻碍

另外，每次去翻找实现的
**代码额外复杂度**
也是缺点之一，所以作为这一点对应的优点，由于你不用每次都改头文件，编译器表示苦一苦人类又算得了什么
**降低编译时间**
才是王道



## 字符解析
### 为什么要解析，不能直接用吗？
可以直接用，但是我想
### 解析逻辑分析
```cpp
std::vector<uint32_t> parseUTF8(const std::string& str) {
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
```
remaining
## 字体载入
```
仅针对 raylib 的字体载入
```
想象一下你是raylib，现在你手上有一个ttf文件，ttf文件内是

## UI排布
