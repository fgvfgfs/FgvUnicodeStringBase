#include "UnicodeString.h" // 实现UnicodeString.h头文件

#pragma warning( disable : 6386 ) // 禁用警告C6386: 读取/写入越界的内存访问检查
using namespace UnicodeStringGroup; // 使用全局命名空间UnicodeStringGroup,避免每次使用类或函数时都要加上命名空间前缀

#pragma region UnicodeString类型对象的构造函数和高危险性操作
// 默认构造函数行为
UnicodeString::UnicodeString( )
{
    // 分配一个UTF-8空字符
    StrValue = new char[1];
    StrValue[0] = '\0';
    isValid = true;
}

// 默认析构函数行为
UnicodeString::~UnicodeString( )
{
    if (StrValue) // 检测UTF-8字符指针是否存在
    {
        delete[ ] StrValue; // 存在则清除UTF-8字符指针指向的数据
        StrValue = nullptr; // 销毁UTF-8字符指针
    }
    DefLength = 0;
    isValid = false;
}

// 拷贝构造函数
UnicodeString::UnicodeString(const UnicodeString &copy)
{
    copy.checkValid( );
    this->DefLength = 0; // 设置默认值
    while (copy.StrValue[this->DefLength] != '\0') // 检测UTF-8字符指针指向的数据是否为UTF-8空字符,如果不是则默认长度自增
        ++this->DefLength;
    this->StrValue = new char[this->DefLength + 1]; // 动态分配UTF-8字符数组,长度+1[因为也得把空字符包含进去 不然会阻塞缓冲区]
    for (size_t i = 0; i <= this->DefLength; ++i) // 将UTF-8字符指针指向的数据全部复制到UTF-8字符数组里(包括空字符\0[也叫终止符])
    {
        this->StrValue[i] = copy.StrValue[i];
    }
    this->StrValue[DefLength] = '\0';
    isValid = true;
}

// 含有const char*类型参数时执行的构造函数
UnicodeString::UnicodeString(const char *u8cp)
{
    if (!u8cp) // 检测传输进来的是否为空指针 如果是 则新建空UTF-8字符数组
    {
        DefLength = 0; // 设置默认值
        StrValue = new char[1]; // 动态分配UTF-8字符数组
        StrValue[0] = '\0'; // 分配一个UTF-8空字符
        isValid = true;
        return;
    }
    DefLength = 0; // 设置默认值
    while (u8cp[DefLength] != '\0') // 检测UTF-8字符指针指向的数据是否为UTF-8空字符,如果不是则默认长度自增
    {
        ++DefLength;
    }
    StrValue = new char[DefLength + 1]; // 动态分配UTF-8字符数组,长度+1[因为也得把空字符包含进去 不然会阻塞缓冲区]
    for (size_t i = 0; i <= DefLength; ++i) // 将UTF-8字符指针指向的数据全部复制到UTF-8字符数组里(包括空字符\0[也叫终止符])
    {
        StrValue[i] = u8cp[i];
    }
    StrValue[DefLength] = '\0';
    isValid = true;
}

// 定义类方法 用于转移对象所有权[注: 转移后源对象会被立即销毁,不能再进行访问]
UnicodeString& UnicodeString::StrObjOwnToMove(UnicodeString &NeedMovestr)
{
    NeedMovestr.checkValid( );
    if (this != &NeedMovestr)
    {
        delete[ ] this->StrValue; // 释放当前对象资源,清除自身对象的持有的字符串
        // 转移资源
        this->StrValue = NeedMovestr.StrValue; // 接管传入的对象的字符串
        this->DefLength = NeedMovestr.DefLength; // 接管传入的对象的字符串长度
        this->isValid = true;
        // 将源对象置于有效但为空的状态
        NeedMovestr.StrValue = nullptr;
        NeedMovestr.DefLength = 0;
        NeedMovestr.isValid = false;
    }
    return *this; // 返回自身
}

// 检测对象是否存在
void UnicodeString::checkValid( ) const
{
    if (!isValid) // 如果不存在则抛出异常
    {
        throw std::logic_error("invalid UnicodeString");
    }
}
#pragma endregion

/*|||||||||||||||||||||||||||||||||||分割线||||||||||||||||||||||||||||||||||||||||*/

#pragma region 只读接口实现
const char* UnicodeString::c_str( ) const
{
    checkValid( );
    return StrValue;
}

size_t UnicodeString::size( ) const
{
    checkValid( );
    return DefLength; // 返回底层字节数
}

size_t UnicodeString::length( ) const
{
    checkValid( );
    size_t charCount = 0;
    for (size_t i = 0; i < DefLength; ++i)
    {
        // 统计不是 UTF-8 延续字节的首字节数量
        if ((StrValue[i] & 0xC0) != 0x80)
        {
            charCount++;
        }
    }
    return charCount; // 返回字符个数
}

bool UnicodeString::empty( ) const
{
    checkValid( );
    return DefLength == 0;
}

// 将字符索引转换为字节偏移量,防止插在汉字的中间
size_t UnicodeString::GetByteOffsetByCharIndex(size_t CharIndex) const
{
    if (CharIndex == 0) return 0;

    size_t currentCharCount = 0;
    size_t byteOffset = 0;

    while (byteOffset < DefLength)
    {
        // UTF-8 延续字节的最高两位总是 10 (即 0x80)
        // 0xC0 是 11000000,与其进行按位与操作,如果结果不是 0x80,说明这是某个字符的首字节
        if ((StrValue[byteOffset] & 0xC0) != 0x80)
        {
            if (currentCharCount == CharIndex)
            {
                return byteOffset; // 找到了目标字符的边界
            }
            currentCharCount++;
        }
        byteOffset++;
    }
    return DefLength; // 越界或等于总字符数,返回末尾
}
#pragma endregion

/*|||||||||||||||||||||||||||||||||||分割线||||||||||||||||||||||||||||||||||||||||*/

#pragma region StrPutOut方法及其重载版本
// 输出自身持有的UTF-8字符方法
void UnicodeString::StrPutOut(ConPutStrAct str_PutFlag) const
{
    checkValid( );
    StrPutOut_GeneralFunc(str_PutFlag); // 调用私有方法 该方法实现通用输出自身持有字符串的操作
}

// 专用于链式调用的类成员方法,用于输出自身拥有的UTF-8字符,同时给予控制是否换行选项
UnicodeString& UnicodeString::StrPutOut_Obj(ConPutStrAct str_PutFlag)
{
    checkValid( );
    StrPutOut_GeneralFunc(str_PutFlag);
    return *this;
}

const UnicodeString& UnicodeString::StrPutOut_Obj(ConPutStrAct str_PutFlag) const
{
    checkValid( );
    StrPutOut_GeneralFunc(str_PutFlag);
    return *this;
}
#pragma endregion

/*|||||||||||||||||||||||||||||||||||分割线||||||||||||||||||||||||||||||||||||||||*/

#pragma region StrPutOutOther方法及其重载版本
// 输出其他非自身持有的UTF-8字符串的方法
void UnicodeString::StrPutOutOther(const char *NputOurStr, ConPutStrAct strPut_Flag) const
{
    checkValid( );
    StrPutOut_OtherGeneralFunc(NputOurStr, strPut_Flag); // 调用私有方法 该方法实现通用输出其他非自身持有字符串的操作
}

// 专用于链式调用的类成员方法,用于输出其他的UTF-8字符
UnicodeString& UnicodeString::StrPutOutOther_Obj(const char *NputOurStr, ConPutStrAct str_PutFlag)
{
    checkValid( );
    StrPutOut_OtherGeneralFunc(NputOurStr, str_PutFlag);
    return *this;
}

const UnicodeString& UnicodeString::StrPutOutOther_Obj(const char *NputOurStr, ConPutStrAct str_PutFlag) const
{
    checkValid( );
    StrPutOut_OtherGeneralFunc(NputOurStr, str_PutFlag);
    return *this;
}
#pragma endregion

/*|||||||||||||||||||||||||||||||||||分割线||||||||||||||||||||||||||||||||||||||||*/

#pragma region UnicodeString类型对象的私有通用方法
// 私有方法 定义通用输出自身持有字符串的操作
void UnicodeString::StrPutOut_GeneralFunc(ConPutStrAct str_PutFlag) const
{
    std::cout << StrValue;
    if (str_PutFlag == ConPutStrAct::NeedEndl) // 检测枚举类型 如果为NeedEndl则进行换行操作
        std::cout << '\n';
}

// 私有方法 定义通用输出其他非自身持有字符串的操作
void UnicodeString::StrPutOut_OtherGeneralFunc(const char *NputOurStr, ConPutStrAct str_PutFlag) const
{
    if (!NputOurStr)
    {
        NputOurStr = "";
    }
    std::cout << NputOurStr;
    if (str_PutFlag == ConPutStrAct::NeedEndl) // 检测枚举类型 如果为NeedEndl则进行换行操作
        std::cout << '\n';
}
#pragma endregion

/*|||||||||||||||||||||||||||||||||||分割线||||||||||||||||||||||||||||||||||||||||*/

#pragma region 特殊全局操作
// 实现自由函数,用于设置控制台为UTF-8,设置本地化为zh_CN.UTF-8并清空重置缓冲区[该效果是全局的,只用使用一次]
void UnicodeStringGroup::StrSetConsoleToUTF8( )
{
#ifdef _WIN32 // 如果是Windows系统 则执行
    system("chcp 65001 > nul"); // 设置控制台为UTF-8	
#endif
    try
    {
        std::locale::global(std::locale("zh_CN.UTF-8")); // 设置本地化为zh_CN.UTF-8
    }
    catch (const std::runtime_error&)
    {
        // 当前环境不支持该 locale,忽略
    }
    std::cout.clear( ); // 重置流的错误状态(恢复错误后的流,例如一个函数期望输入数字而用户输入了字符 这个时候流就会有错误状态)
    std::cout.flush( ); // 强制刷新缓冲区
    // 清除缓冲区并同步
}
#pragma endregion

/*|||||||||||||||||||||||||||||||||||分割线||||||||||||||||||||||||||||||||||||||||*/

#pragma region StrIns方法及其重载版本
// 私有方法 定义通用字符串操作函数精简代码并提升复用性
void UnicodeString::StrInsGeneralFunc(const char *u8cpIns, InsStrPosition str_InsFlag)
{
    // 以下全变成局部变量,用完自动销毁,不再需要 delete[] 成员
    size_t insLen = 0;
    if (!u8cpIns) // 检测传输进来的是否为空指针 如果是 则默认插入空
    {
        return;
    }

    while (u8cpIns[insLen] != '\0') // 检测UTF-8字符指针指向的数据是否为UTF-8空字符,如果不是则默认长度自增
    {
        ++insLen;
    }

    size_t finalLen = DefLength + insLen; // 总长度 = 默认长度[原自身持有的] + 需要插入的字符串的长度; 注意这两的长度都不计入终止符 也就是空字符\0
    char* finalStr = new char[finalLen + 1]; // 动态分配内存,总长度+1是为了包含终止符

    size_t k = 0;
    if (str_InsFlag == InsStrPosition::InFirst)
    {
        for (size_t i = 0; i < insLen; ++i) finalStr[k++] = u8cpIns[i];
        for (size_t i = 0; i < DefLength; ++i) finalStr[k++] = StrValue[i];
    }
    else if (str_InsFlag == InsStrPosition::InLast)
    {
        for (size_t i = 0; i < DefLength; ++i) finalStr[k++] = StrValue[i];
        for (size_t i = 0; i < insLen; ++i) finalStr[k++] = u8cpIns[i];
    }
    else // 如果 没有声明任何情况 则执行默认操作 在末尾插入字符串
    {
        for (size_t i = 0; i < DefLength; ++i) finalStr[k++] = StrValue[i];
        for (size_t i = 0; i < insLen; ++i) finalStr[k++] = u8cpIns[i];
    }
    finalStr[finalLen] = '\0';

    if (StrValue) // 检测UTF-8字符指针是否存在
    {
        delete[ ] StrValue; // 存在则清除UTF-8字符指针指向的数据
        StrValue = nullptr; // 设置StrValue为空指针
    }

    StrValue = finalStr; // 直接接管分配好的内存
    DefLength = finalLen; // 更新长度
}

// 定义类方法,用于插入字符串 同时给予控制选项
char* UnicodeString::StrIns(const char *u8cpIns, InsStrPosition str_InsFlag)
{
    checkValid( );
    StrInsGeneralFunc(u8cpIns, str_InsFlag);
    return StrValue;
}

// 插入并复制字符串对象[不会修改源字符串,这是深拷贝]
UnicodeString& UnicodeString::StrInsAndCopy(const char *u8cpIns, InsStrPosition str_InsFlag)
{
    checkValid( );
    StrInsGeneralFunc(u8cpIns, str_InsFlag);
    return *this;
}

// 限定在指定字符索引位置插入 (原先的 InsPosition 是字节索引,现在是真正的字符索引)
char* UnicodeString::StrIns(const char *u8cpIns, size_t CharIndex)
{
    checkValid( );

    if (!u8cpIns) // 检测传输进来的是否为空指针 如果是 则直接返回
    {
        return StrValue;
    }

    // 1. 获取安全的字节偏移量 (即使传入的 CharIndex 落在汉字中间,也会自动修正到该字符的起始字节)
    size_t byteOffset = GetByteOffsetByCharIndex(CharIndex);

    // 2. 如果实际算出来的字节偏移是 0 或者等于末尾,直接走高效的追加逻辑
    if (byteOffset == 0)
    {
        StrInsGeneralFunc(u8cpIns, InsStrPosition::InFirst);
        return StrValue;
    }
    else if (byteOffset >= DefLength)
    {
        StrInsGeneralFunc(u8cpIns, InsStrPosition::InLast);
        return StrValue;
    }

    size_t insLen = 0;
    while (u8cpIns[insLen] != '\0') // 检测UTF-8字符指针指向的数据是否为UTF-8空字符,如果不是则默认长度自增
    {
        ++insLen;
    }

    // 走到这里说明是在合法的中间位置插入
    size_t tempStrLen = byteOffset;
    char* tempStr = new char[tempStrLen + 1];
    for (size_t i = 0; i < tempStrLen; ++i)
    {
        tempStr[i] = StrValue[i];
    }
    tempStr[tempStrLen] = '\0';

    size_t charsToCopy = DefLength - tempStrLen;
    size_t tempTwoStrLen = charsToCopy;
    char* tempTwoStr = new char[tempTwoStrLen + 1];
    for (size_t i = 0; i < tempTwoStrLen; ++i)
    {
        tempTwoStr[i] = StrValue[tempStrLen + i];
    }
    tempTwoStr[tempTwoStrLen] = '\0';

    size_t finalLen = tempStrLen + insLen + tempTwoStrLen;
    char* finalStr = new char[finalLen + 1]; // 动态分配内存,总长度+1是为了包含终止符

    size_t ls = 0;
    for (size_t j = 0; j < tempStrLen; ++j) finalStr[ls++] = tempStr[j];
    for (size_t j = 0; j < insLen; ++j) finalStr[ls++] = u8cpIns[j];
    for (size_t j = 0; j < tempTwoStrLen; ++j) finalStr[ls++] = tempTwoStr[j];

    finalStr[finalLen] = '\0';

    // 清理之前分配的局部临时内存
    delete[ ] tempStr;
    delete[ ] tempTwoStr;

    if (StrValue) // 检测UTF-8字符指针是否存在
    {
        delete[ ] StrValue; // 存在则清除UTF-8字符指针指向的数据
        StrValue = nullptr; // 设置StrValue为空指针
    }
    StrValue = finalStr;
    DefLength = finalLen;

    return StrValue;
}
#pragma endregion

/*|||||||||||||||||||||||||||||||||||分割线||||||||||||||||||||||||||||||||||||||||*/

#pragma region 适用于UnicodeString类型对象的运算符重载
// 赋值运算符重载 适用于 Str = "我是新字符串"; 的情况
void UnicodeString::operator=(const char *u8cp)
{
    checkValid( );
    if (!u8cp) // 检测传输进来的是否为空指针 如果是 则新建空UTF-8字符数组
    {
        this->DefLength = 0; // 设置默认值
        delete[ ] this->StrValue; // 存在则清除UTF-8字符指针指向的数据
        this->StrValue = new char[1]; // 动态分配UTF-8字符数组
        this->StrValue[0] = '\0'; // 分配一个UTF-8空字符
        return;
    }
    if (StrValue) // 检测UTF-8字符指针是否存在
    {
        delete[ ] this->StrValue; // 存在则清除UTF-8字符指针指向的数据
        this->StrValue = nullptr; // 设置StrValue为空指针
        this->DefLength = 0; // 设置默认值
    }
    this->DefLength = 0; // 设置默认值
    while (u8cp[this->DefLength] != '\0') // 检测UTF-8字符指针指向的数据是否为UTF-8空字符,如果不是则默认长度自增
    {
        ++this->DefLength;
    }
    StrValue = new char[this->DefLength + 1]; // 动态分配UTF-8字符数组,长度+1[因为也得把空字符包含进去 不然会阻塞缓冲区]
    for (size_t i = 0; i <= this->DefLength; ++i) // 将UTF-8字符指针指向的数据全部复制到UTF-8字符数组里(包括空字符\0[也叫终止符])
    {
        this->StrValue[i] = u8cp[i];
    }
    this->StrValue[this->DefLength] = '\0';
}

// 加法运算符重载 适用于 Str + Str2; 的情况
UnicodeString& UnicodeString::operator+(const UnicodeString &str)
{
    checkValid( );
    str.checkValid( );
    size_t insLen = 0;
    while (str.StrValue[insLen] != '\0') // 检测UTF-8字符指针指向的数据是否为UTF-8空字符,如果不是则默认长度自增
    {
        ++insLen;
    }

    size_t finalLen = DefLength + insLen; // 总长度 = 默认长度[原自身持有的] + 需要插入的字符串的长度; 注意这两的长度都不计入终止符 也就是空字符\0
    char* finalStr = new char[finalLen + 1]; // 动态分配内存,总长度+1是为了包含终止符

    size_t k = 0;
    for (size_t i = 0; i < DefLength; ++i)
    {
        finalStr[k++] = StrValue[i];
    }
    for (size_t i = 0; i < insLen; ++i)
    {
        finalStr[k++] = str.StrValue[i];
    }
    finalStr[finalLen] = '\0';

    if (StrValue) // 检测UTF-8字符指针是否存在
    {
        delete[ ] StrValue; // 存在则清除UTF-8字符指针指向的数据
        StrValue = nullptr; // 设置StrValue为空指针
    }
    StrValue = finalStr;
    DefLength = finalLen;
    return *this;
}

// 赋值运算符重载 适用于 Str = Str2; 的情况
void UnicodeString::operator=(const UnicodeString &str)
{
    checkValid( );
    str.checkValid( );
    if (this == &str) return; // 自我赋值检查,如果是同一个对象则直接返回
    if (StrValue) // 检测UTF-8字符指针是否存在
    {
        delete[ ] this->StrValue; // 存在则清除UTF-8字符指针指向的数据
        this->StrValue = nullptr; // 设置StrValue为空指针
        this->DefLength = 0; // 设置默认值
    }
    this->DefLength = 0; // 设置默认值
    while (str.StrValue[this->DefLength] != '\0') // 检测UTF-8字符指针指向的数据是否为UTF-8空字符,如果不是则默认长度自增
    {
        ++this->DefLength;
    }
    StrValue = new char[this->DefLength + 1]; // 动态分配UTF-8字符数组,长度+1[因为也得把空字符包含进去 不然会阻塞缓冲区]
    for (size_t i = 0; i <= this->DefLength; ++i) // 将UTF-8字符指针指向的数据全部复制到UTF-8字符数组里(包括空字符\0[也叫终止符])
    {
        this->StrValue[i] = str.StrValue[i];
    }
    this->StrValue[this->DefLength] = '\0';
}

bool UnicodeString::operator==(const UnicodeString &str) const // 等于运算符重载 适用于 Str == Str2; 的情况
{
    if (this == &str) return true; // 自我比较检查,如果是同一个对象则直接返回true
    checkValid( );
    str.checkValid( );
    if (this->DefLength != str.DefLength) // 如果长度不相等则直接返回false
    {
        return false;
    }
    return memcmp(this->StrValue, str.StrValue, this->DefLength) == 0;
    // 使用memcmp比较两个字符串的内容,如果相等则返回true,否则返回false
}

bool UnicodeString::operator!=(const UnicodeString &str) const // 不等于运算符重载 适用于 Str != Str2; 的情况
{
    return !(*this == str); // 调用等于运算符重载,如果相等则返回false,否则返回true
}
#pragma endregion

/*|||||||||||||||||||||||||||||||||||分割线||||||||||||||||||||||||||||||||||||||||*/

#pragma region Unicode库的有效性检验
void UnicodeStringGroup::stdTestCode( ) // 定义自由函数,用于测试UnicodeString类的有效性
{
    // 1. 初始化控制台 (跨平台尝试设置UTF-8环境)
    StrSetConsoleToUTF8( );
    std::cout << "========== 测试开始 ==========\n";

    // 2. 基础构造与只读接口
    std::cout << "\n--- 测试 1: 基础构造与只读接口 ---\n";
    const UnicodeString constStr("你好Hello"); // 测试 const 对象
    constStr.StrPutOut(ConPutStrAct::NeedEndl); // 应该可以正常输出
    std::cout << "size() (字节数): " << constStr.size( ) << "\n";
    std::cout << "length() (字符数): " << constStr.length( ) << "\n";
    std::cout << "empty(): " << (constStr.empty( ) ? "true" : "false") << "\n";
    std::cout << "c_str(): " << constStr.c_str( ) << "\n";

    // 3. 拷贝构造与赋值运算符
    std::cout << "\n--- 测试 2: 拷贝与赋值 ---\n";
    UnicodeString s2("World");
    UnicodeString s4(s2); // 拷贝构造
    s4.StrPutOut(ConPutStrAct::NeedEndl);
    s2 = "直接赋值的C字符串"; // const char* 赋值
    s2.StrPutOut(ConPutStrAct::NeedEndl);

    // 4. 索引插入(边界与中间)
    std::cout << "\n--- 测试 3: 索引插入测试 (字符个数) ---\n";
    UnicodeString s8("ABCDE");
    std::cout << "原字符串: "; s8.StrPutOut(ConPutStrAct::NeedEndl);

    std::cout << "在字符索引 0 插入 '>>' : ";
    s8.StrIns(">>", 0); // 预期: >>ABCDE
    s8.StrPutOut(ConPutStrAct::NeedEndl);

    std::cout << "在字符索引 4 插入 '[]' : ";
    s8.StrIns("[]", 4); // 预期: >>AB[]CDE
    s8.StrPutOut(ConPutStrAct::NeedEndl);

    // 5. 多字节字符边界与越界测试 (中英文混合)
    std::cout << "\n--- 测试 4: 多字节字符边界测试 (中英混合) ---\n";
    UnicodeString s9("A你B好C");
    std::cout << "原字符串: "; s9.StrPutOut(ConPutStrAct::NeedEndl);

    // 首部插入
    std::cout << "在字符索引 0 (首部) 插入 '[首]' : ";
    s9.StrIns("[首]", 0); // 预期: [首]A你B好C
    s9.StrPutOut(ConPutStrAct::NeedEndl);

    // 中间插入
    std::cout << "在字符索引 4 (中间) 插入 '[中]' : ";
    s9.StrIns("[中]", 4); // 预期: [首]A你[中]B好C
    s9.StrPutOut(ConPutStrAct::NeedEndl);

    // 越界巨大索引
    std::cout << "在字符索引 999 (越界) 插入 '[越界]' : ";
    s9.StrIns("[越界]", 999); // 预期追加到末尾
    s9.StrPutOut(ConPutStrAct::NeedEndl);

    // 6. 运算符重载 + (拼接)
    std::cout << "\n--- 测试 5: 字符串拼接与移动 ---\n";
    UnicodeString a("你好，");
    UnicodeString b("世界！");
    a + b;
    std::cout << "a + b 后: "; a.StrPutOut(ConPutStrAct::NeedEndl);

    UnicodeString src("移动");
    a.StrObjOwnToMove(src);

    // 7. 链式调用
    std::cout << "\n--- 测试 6: 链式调用 ---\n";
    UnicodeString chain("链式");
    chain.StrPutOut_Obj(ConPutStrAct::NotEndl)
        .StrPutOutOther_Obj(" 调用", ConPutStrAct::NotEndl)
        .StrPutOut_Obj(ConPutStrAct::NeedEndl);

    std::cout << "\n========== 测试结束 ==========\n";
}
#pragma endregion

/*|||||||||||||||||||||||||||||||||||分割线||||||||||||||||||||||||||||||||||||||||*/