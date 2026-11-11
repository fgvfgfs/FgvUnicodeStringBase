export module UnicodeString;

import std;

export namespace UnicodeStringGroup
{
	void StrSetConsoleToUTF8( ); // 定义自由函数, 用于设置控制台为UTF-8, 设置本地化为zh_CN.UTF-8并清空重置缓冲区(该效果是全局的, 只用使用一次)

	enum class ConPutStrAct // 控制台字符输出时行动/操作 (强类型枚举)
	{
		NotEndl, NeedEndl // 不需要换行, 需要换行
	};

	enum class InsStrPosition // 控制插入字符操作 (强类型枚举)
	{
		InLast, InFirst // 在末尾(插入), 在首端(插入)
	};

	class UnicodeString // 定义UnicodeString类 用于处理UTF-8字符
	{
	public: // 定义访问修饰符为公共的
		UnicodeString( ) = default; // 默认构造函数
		UnicodeString(const char8_t *u8cp); // 定义参数为const char8_t*时的构造函数重载
		UnicodeString(const UnicodeString &copy) = default; // 拷贝构造函数
		UnicodeString& operator=(const UnicodeString &str) = default; // 赋值运算符重载

		// --- 只读接口 ---
		const char8_t* c_str( ) const; // 获取底层C风格字符串指针
		size_t size( ) const; // 获取当前字节数
		size_t length( ) const; // 获取当前字符个数
		bool empty( ) const; // 判断字符串是否为空
		// ---------------

		void StrPutOut(ConPutStrAct str_PutFlag = ConPutStrAct::NeedEndl) const; // 定义类方法, 用于输出自身拥有的UTF-8字符, 同时给予控制是否换行选项
		void StrPutOutOther(const char8_t *NputOurStr, ConPutStrAct str_PutFlag = ConPutStrAct::NeedEndl) const; // 定义类方法, 用于输出其他的UTF-8字符

		UnicodeString& StrPutOut_Obj(ConPutStrAct str_PutFlag = ConPutStrAct::NeedEndl);
		const UnicodeString& StrPutOut_Obj(ConPutStrAct str_PutFlag = ConPutStrAct::NeedEndl) const;

		UnicodeString& StrPutOutOther_Obj(const char8_t *NputOurStr, ConPutStrAct str_PutFlag = ConPutStrAct::NeedEndl);
		const UnicodeString& StrPutOutOther_Obj(const char8_t *NputOurStr, ConPutStrAct str_PutFlag = ConPutStrAct::NeedEndl) const;

		// 其实C/C++应该叫做函数, 但是咱C#写习惯了 所以就习惯叫成为 方法 了
		const char8_t* StrIns(const char8_t *u8cpIns, InsStrPosition str_InsFlag = InsStrPosition::InLast); // 定义类方法, 用于插入字符串 同时给予控制选项
		UnicodeString& StrInsAndCopy(const char8_t *u8cpIns, InsStrPosition str_InsFlag = InsStrPosition::InLast); // 插入并复制字符串对象[不会修改源字符串, 这是深拷贝]
		void operator=(const char8_t *u8cp); // 赋值运算符重载 适用于 Str = u8"我是新字符串"; 的情况
		UnicodeString& operator+(const UnicodeString &str); // 加法运算符重载 适用于 Str + Str2; 的情况
		bool operator==(const UnicodeString &str) const; // 等于运算符重载
		bool operator!=(const UnicodeString &str) const; // 不等于运算符重载
		UnicodeString& StrObjOwnToMove(UnicodeString &NeedMovestr); // 定义类方法 用于转移对象所有权

		const char8_t* StrIns(const char8_t *u8cpIns, size_t CharIndex); // 定义类方法, 用于插入字符串 限定在指定字符索引位置插入

	private: // 定义访问修饰符为私有的
		std::u8string StrValue = u8""; // 底层使用 std::u8string 存储, 自动管理内存
		ConPutStrAct Str_PutFlag = ConPutStrAct::NotEndl; // 默认为不需要换行

		void StrPutOut_GeneralFunc(ConPutStrAct str_PutFlag = ConPutStrAct::NeedEndl) const; // 私有方法 定义通用输出自身持有字符串的操作
		void StrPutOut_OtherGeneralFunc(const char8_t *NputOurStr, ConPutStrAct str_PutFlag = ConPutStrAct::NeedEndl) const; // 私有方法 定义通用输出其他非自身持有字符串的操作
		void StrInsGeneralFunc(const char8_t *u8cpIns, InsStrPosition str_InsFlag = InsStrPosition::InLast); // 私有方法 定义通用字符串操作函数
		size_t GetByteOffsetByCharIndex(size_t CharIndex) const; // 私有方法 将字符索引转换为字节偏移量
	};

	void stdTestCode( ); // 定义自由函数, 用于测试UnicodeString类的有效性
}