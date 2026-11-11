#pragma once // 仅编译一次
#include <cstddef> // 包含cstddef头文件,用于使用size_t类型
#include <exception> // 包含exception头文件,用于使用异常处理机制
#include <iostream> // 包含iostream头文件,用于使用输入输出流
#include <locale> // 包含locale头文件,用于使用本地化设置
#include <cstdlib> // 包含cstdlib头文件,用于使用系统函数
#include <cstring> // 包含cstring头文件,用于使用字符串处理函数
#include <stdexcept> // 包含stdexcept头文件,用于使用标准异常类

namespace UnicodeStringGroup
{
	void StrSetConsoleToUTF8( ); // 定义自由函数,用于设置控制台为UTF-8,设置本地化为zh_CN.UTF-8并清空重置缓冲区[该效果是全局的,只用使用一次]

	enum class ConPutStrAct // 控制台字符输出时行动/操作 (强类型枚举)
	{
		NotEndl, NeedEndl // 不需要换行,需要换行
	};

	enum class InsStrPosition // 控制插入字符操作 (强类型枚举)
	{
		InLast, InFirst // 在末尾(插入),在首端(插入)
	};

	class UnicodeString // 定义UnicodeString类 用于处理UTF-8字符
	{
	public: // 定义访问修饰符为公共的
		UnicodeString( ); // 定义默认构造函数
		~UnicodeString( ); // 定义默认析构函数
		UnicodeString(const UnicodeString &copy); // 定义拷贝构造函数
		UnicodeString(const char *u8cp); // 定义参数为const char*时的构造函数重载
		void StrPutOut(ConPutStrAct str_PutFlag = ConPutStrAct::NeedEndl); // 定义类方法,用于输出自身拥有的UTF-8字符,同时给予控制是否换行选项
		void StrPutOutOther(const char *NputOurStr, ConPutStrAct str_PutFlag = ConPutStrAct::NeedEndl); // 定义类方法,用于输出其他的UTF-8字符
		UnicodeString& StrPutOut_Obj(ConPutStrAct str_PutFlag = ConPutStrAct::NeedEndl); // 专用于链式调用的类成员方法,用于输出自身拥有的UTF-8字符,同时给予控制是否换行选项
		UnicodeString& StrPutOutOther_Obj(const char *NputOurStr, ConPutStrAct str_PutFlag = ConPutStrAct::NeedEndl); // 专用于链式调用的类成员方法,用于输出其他的UTF-8字符
		// 其实C/C++应该叫做函数,但是咱C#写习惯了 所以就习惯叫成为 方法 了
		char* StrIns(const char *u8cpIns, InsStrPosition str_InsFlag = InsStrPosition::InLast); // 定义类方法,用于插入字符串 同时给予控制选项
		UnicodeString& StrInsAndCopy(const char *u8cpIns, InsStrPosition str_InsFlag = InsStrPosition::InLast); // 插入并复制字符串对象[不会修改源字符串,这是深拷贝]
		void operator=(const char *u8cp); // 赋值运算符重载 适用于 Str = "我是新字符串"; 的情况
		void operator+(const UnicodeString &str); // 加法运算符重载 适用于 Str + Str2; 的情况
		void operator=(const UnicodeString &str); // 赋值运算符重载 适用于 Str = Str2; 的情况
		bool operator==(const UnicodeString &str) const; // 等于运算符重载 适用于 Str == Str2; 的情况
		bool operator!=(const UnicodeString &str) const; // 不等于运算符重载 适用于 Str != Str2; 的情况
		UnicodeString& StrObjOwnToMove(UnicodeString &NeedMovestr); // 定义类方法 用于转移对象所有权[注: 转移后源对象会被立即销毁,不能再进行访问]

		// TODO StrIns定义还未实现
		char* StrIns(const char *u8cpIns, size_t CharIndex); // 定义类方法,用于插入字符串 限定在指定字符索引位置插入
	private: // 定义访问修饰符为私有的
		ConPutStrAct Str_PutFlag = ConPutStrAct::NotEndl; // 默认为不需要换行
		size_t DefLength = 0; // 定义默认长度,未初始化 确切值在运行时才能确定 用于动态内存分配
		char *StrValue = nullptr; // 动态UTF-8字符数组分配,长度是在运行时确定的,本质上是个指向动态UTF-8字符数组的UTF-8字符指针
		bool isValid = false; // 表示该对象是否有效/存在

		// 以下为被精简的临时变量 (原本占用了超过 70 字节):
		// InsStrLength, InsStrValue, FinalStrLength, FinalStrValue, 
		// TempStrLength, TempStrValue, TempTwoStrLength, TempTwoStrValue, 
		// Str_InsFlag, Str_InsPositionFlag 已全部移入对应函数的局部变量中.

		void StrPutOut_GeneralFunc(ConPutStrAct str_PutFlag = ConPutStrAct::NeedEndl); // 私有方法 定义通用输出自身持有字符串的操作
		void StrPutOut_OtherGeneralFunc(const char *NputOurStr, ConPutStrAct str_PutFlag = ConPutStrAct::NeedEndl); // 私有方法 定义通用输出其他非自身持有字符串的操作
		void StrInsGeneralFunc(const char *u8cpIns, InsStrPosition str_InsFlag = InsStrPosition::InLast); // 私有方法 定义通用字符串操作函数精简代码并提升复用性
		void checkValid( ) const; // 检查该对象是否有效/存在
		size_t GetByteOffsetByCharIndex(size_t CharIndex) const; // 新增辅助函数: 将字符个数索引转换为真实的字节偏移量
	};

	void stdTestCode( ); // 定义自由函数,用于测试UnicodeString类的有效性
}