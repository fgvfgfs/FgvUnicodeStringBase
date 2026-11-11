# FgvUnicodeStringBase

![C++14](https://img.shields.io/badge/C%2B%2B-14-blue.svg)
![C++26](https://img.shields.io/badge/C%2B%2B-26-blue.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)

一个轻量级的 C++ UTF-8 字符串处理库. 旨在解决原生 `std::string` 和 `std::u8string` 在变长编码下, 按字节索引极易导致中文等宽字符被截断而产生乱码的底层痛点.

同时提供 C++14 与 C++26 两个版本, 兼顾老旧项目的兼容性与现代 C++ 的安全.

## 核心特性

- **字符级安全索引**: 提供基于 UTF-8 字符个数 (而非底层字节数) 的插入操作, 彻底杜绝汉字被劈开导致的乱码.
- **双标准并存**:
  - **C++26 版本**: 基于 `std::u8string` 与 C++ Modules 模块化构建, 遵守 Rule of Zero.
  - **C++14 版本**: 传统 `.h`/`.cpp` 分离, 手动管理内存, 类体积被极致精简至 24 字节, 无任何内存泄漏风险.
- **链式调用支持**: 提供 `StrPutOut_Obj` 等流式接口, 让输出和拼接代码如流水般顺畅.
- **跨平台控制台适配**: 内置 `StrSetConsoleToUTF8()` 函数, 自动处理 Windows 控制台的代码页 (`chcp 65001`) 与本地化设置.

## 快速开始

### C++26 版本 (要求编译器支持 Modules 与 `char8_t`)

```cpp
import UnicodeString;
using namespace UnicodeStringGroup;

int main()
{
    // 跨平台设置控制台 UTF-8
    StrSetConsoleToUTF8();

    UnicodeString str(u8"你好世界");
    
    // 在"你"和"好"之间插入 (按字符索引 1, 底层自动计算字节偏移量)
    str.StrIns(u8"(中间)", 1); 
    str.StrPutOut(ConPutStrAct::NeedEndl); // 输出: 你(中间)好世界

    return 0;
}

### C++14 版本 (要求编译器支持 C++14 及以上)
```cpp
#include "UnicodeString.h"
using namespace UnicodeStringGroup;

int main()
{
    StrSetConsoleToUTF8();
    
    UnicodeString str("你好世界");
    str.StrIns("(中间)", 1); 
    str.StrPutOut(ConPutStrAct::NeedEndl); // 输出: 你(中间)好世界
    
    return 0;
}