module UnicodeString;

import std;

// 必须加上命名空间包裹，这样定义的函数才会归属于 UnicodeStringGroup
namespace UnicodeStringGroup
{
#pragma region UnicodeString类型对象的构造函数和高危险性操作
	// 含有 const char8_t* 类型参数时执行的构造函数
	UnicodeString::UnicodeString(const char8_t *u8cp)
	{
		if (u8cp) // 检测传输进来的是否为空指针
		{
			StrValue = u8cp; // 如果不为空 则赋值
		}
	}
#pragma endregion

/*|||||||||||||||||||||||||||||||||||分割线||||||||||||||||||||||||||||||||||||||||*/

#pragma region StrPutOut方法及其重载版本
	// 输出自身持有的 UTF-8 字符方法
	void UnicodeString::StrPutOut(ConPutStrAct str_PutFlag)
	{
		StrPutOut_GeneralFunc(str_PutFlag); // 调用私有方法 该方法实现通用输出自身持有字符串的操作
	}

	// 专用于链式调用的类成员方法 用于输出自身拥有的 UTF-8 字符
	UnicodeString& UnicodeString::StrPutOut_Obj(ConPutStrAct str_PutFlag)
	{
		StrPutOut_GeneralFunc(str_PutFlag);
		return *this;
	}
#pragma endregion

/*|||||||||||||||||||||||||||||||||||分割线||||||||||||||||||||||||||||||||||||||||*/

#pragma region StrPutOutOther方法及其重载版本
	// 输出其他非自身持有的 UTF-8 字符串的方法
	void UnicodeString::StrPutOutOther(const char8_t *NputOurStr, ConPutStrAct strPut_Flag)
	{
		StrPutOut_OtherGeneralFunc(NputOurStr, strPut_Flag); // 调用私有方法 该方法实现通用输出其他非自身持有字符串的操作
	}

	// 专用于链式调用的类成员方法 用于输出其他的 UTF-8 字符
	UnicodeString& UnicodeString::StrPutOutOther_Obj(const char8_t *NputOurStr, ConPutStrAct str_PutFlag)
	{
		StrPutOut_OtherGeneralFunc(NputOurStr, str_PutFlag);
		return *this;
	}
#pragma endregion

/*|||||||||||||||||||||||||||||||||||分割线||||||||||||||||||||||||||||||||||||||||*/

#pragma region UnicodeString类型对象的私有通用方法
	// 私有方法 定义通用输出自身持有字符串的操作
	void UnicodeString::StrPutOut_GeneralFunc(ConPutStrAct str_PutFlag)
	{
		// C++20/23 中 std::cout 对 u8string 的支持还不完善, 转为 const char* 输出
		std::cout << reinterpret_cast<const char*>(StrValue.c_str( ));
		if (str_PutFlag == ConPutStrAct::NeedEndl) // 检测枚举类型 如果为 NeedEndl 则进行换行操作
		{
			std::cout << '\n';
		}
	}

	// 私有方法 定义通用输出其他非自身持有字符串的操作
	void UnicodeString::StrPutOut_OtherGeneralFunc(const char8_t *NputOurStr, ConPutStrAct str_PutFlag)
	{
		if (!NputOurStr) // 检测传输进来的是否为空指针
		{
			NputOurStr = u8""; // 如果是 则赋值为空字符串防止崩溃
		}
		std::cout << reinterpret_cast<const char*>(NputOurStr); // 转为 const char* 输出
		if (str_PutFlag == ConPutStrAct::NeedEndl) // 检测枚举类型 如果为 NeedEndl 则进行换行操作
		{
			std::cout << '\n';
		}
	}
#pragma endregion

/*|||||||||||||||||||||||||||||||||||分割线||||||||||||||||||||||||||||||||||||||||*/

#pragma region 特殊全局操作
	// 实现自由函数 用于设置控制台为 UTF-8, 设置本地化为 zh_CN.UTF-8 并清空重置缓冲区[该效果是全局的, 只用使用一次]
	void StrSetConsoleToUTF8( )
	{
#ifdef _WIN32 // 如果是Windows系统 则执行
		system("chcp 65001 > nul"); // 设置控制台为 UTF-8
#endif
		try
		{
			std::locale::global(std::locale("zh_CN.UTF-8")); // 设置本地化为 zh_CN.UTF-8
		}
		catch (const std::runtime_error&)
		{
			// 当前环境不支持该 locale 忽略
		}
		std::cout.clear( ); // 重置流的错误状态(恢复错误后的流, 例如一个函数期望输入数字而用户输入了字符 这个时候流就会有错误状态)
		std::cout.flush( ); // 强制刷新缓冲区
	}
#pragma endregion

/*|||||||||||||||||||||||||||||||||||分割线||||||||||||||||||||||||||||||||||||||||*/

#pragma region StrIns方法及其重载版本
	// 私有方法 定义通用字符串操作函数精简代码并提升复用性
	void UnicodeString::StrInsGeneralFunc(const char8_t *u8cpIns, InsStrPosition str_InsFlag)
	{
		if (!u8cpIns) return; // 检测传输进来的是否为空指针 如果是 则直接返回

		std::u8string insStr(u8cpIns);
		if (str_InsFlag == InsStrPosition::InFirst) // 如果是 声明在首端插入
		{
			StrValue.insert(0, insStr); // 借助 std::u8string 的 insert 方法在开头插入
		}
		else // 如果 没有声明任何情况 则执行默认操作 在末尾插入字符串
		{
			StrValue.append(insStr); // 借助 std::u8string 的 append 方法在末尾追加
		}
	}

	// 定义类方法 用于插入字符串 同时给予控制选项
	const char8_t* UnicodeString::StrIns(const char8_t *u8cpIns, InsStrPosition str_InsFlag)
	{
		StrInsGeneralFunc(u8cpIns, str_InsFlag);
		return StrValue.c_str( ); // 返回底层 C 风格字符串指针
	}

	// 插入并复制字符串对象[不会修改源字符串, 这是深拷贝]
	UnicodeString& UnicodeString::StrInsAndCopy(const char8_t *u8cpIns, InsStrPosition str_InsFlag)
	{
		StrInsGeneralFunc(u8cpIns, str_InsFlag);
		return *this; // 返回自身引用支持链式调用
	}

	// 私有方法 将字符索引转换为字节偏移量
	size_t UnicodeString::GetByteOffsetByCharIndex(size_t CharIndex) const
	{
		if (CharIndex == 0) return 0; // 如果索引为 0 直接返回偏移量 0

		size_t currentCharCount = 0; // 当前遍历到的字符个数
		size_t byteOffset = 0; // 当前遍历到的字节偏移量
		size_t totalBytes = StrValue.length( ); // 字符串总字节数

		while (byteOffset < totalBytes)
		{
			// UTF-8 延续字节的最高两位总是 10 (即 0x80)
			// 0xC0 是 11000000, 与其进行按位与操作, 如果结果不是 0x80, 说明这是某个字符的首字节
			if ((StrValue[byteOffset] & 0xC0) != 0x80)
			{
				if (currentCharCount == CharIndex)
				{
					return byteOffset; // 找到目标字符的起始字节边界
				}
				currentCharCount++; // 字符个数自增
			}
			byteOffset++; // 字节偏移量自增
		}
		return totalBytes; // 越界或等于总字符数 返回末尾偏移量
	}

	// 定义类方法 用于插入字符串 限定在指定字符索引位置插入
	const char8_t* UnicodeString::StrIns(const char8_t *u8cpIns, size_t CharIndex)
	{
		if (!u8cpIns) return StrValue.c_str( ); // 检测传输进来的是否为空指针 如果是 则直接返回

		// 1. 获取安全的字节偏移量
		size_t byteOffset = GetByteOffsetByCharIndex(CharIndex);

		// 2. 如果实际算出来的字节偏移是 0 或者大于等于末尾, 直接走高效的追加逻辑
		if (byteOffset == 0)
		{
			StrInsGeneralFunc(u8cpIns, InsStrPosition::InFirst); // 走首端插入
		}
		else if (byteOffset >= StrValue.length( ))
		{
			StrInsGeneralFunc(u8cpIns, InsStrPosition::InLast); // 走末尾追加
		}
		else // 走到这里说明是在合法的中间位置插入
		{
			// 借助 std::u8string 的 insert 方法直接在安全边界插入
			StrValue.insert(byteOffset, u8cpIns);
		}
		return StrValue.c_str( );
	}
#pragma endregion

/*|||||||||||||||||||||||||||||||||||分割线||||||||||||||||||||||||||||||||||||||||*/

#pragma region 适用于UnicodeString类型对象的运算符重载
	// 赋值运算符重载 适用于 Str = u8"我是新字符串"; 的情况
	void UnicodeString::operator=(const char8_t *u8cp)
	{
		StrValue = u8cp ? u8cp : u8""; // 检测是否为空指针 如果不是 则赋值 如果是 则赋空字符串
	}

	// 加法运算符重载 适用于 Str + Str2; 的情况
	void UnicodeString::operator+(const UnicodeString &str)
	{
		StrValue.append(str.StrValue); // 将传入对象的底层字符串追加到自身末尾
	}

	// 等于运算符重载 适用于 Str == Str2; 的情况
	bool UnicodeString::operator==(const UnicodeString &str) const
	{
		return StrValue == str.StrValue; // 直接利用 std::u8string 的 == 运算符进行比较
	}

	// 不等于运算符重载 适用于 Str != Str2; 的情况
	bool UnicodeString::operator!=(const UnicodeString &str) const
	{
		return !(*this == str); // 调用等于运算符重载 如果相等则返回 false 否则返回 true
	}

	// 定义类方法 用于转移对象所有权[注: 转移后源对象会被立即销毁, 不能再进行访问]
	UnicodeString& UnicodeString::StrObjOwnToMove(UnicodeString &NeedMovestr)
	{
		if (this != &NeedMovestr) // 防止自我移动
		{
			StrValue = std::move(NeedMovestr.StrValue); // 接管传入的对象的字符串资源
			NeedMovestr.StrValue.clear( ); // 将源对象置于有效但为空的状态
		}
		return *this; // 返回自身
	}
#pragma endregion

/*|||||||||||||||||||||||||||||||||||分割线||||||||||||||||||||||||||||||||||||||||*/

#pragma region Unicode库的有效性检验
	// 定义自由函数 用于测试 UnicodeString 类的有效性
	void stdTestCode( )
	{
		StrSetConsoleToUTF8( ); // 初始化控制台(跨平台尝试设置 UTF-8 环境)
		std::cout << "========== 测试开始 ==========\n";

		// 测试 1: 基础构造与输出
		std::cout << "\n--- 测试 1: 基础构造与输出 ---\n";
		UnicodeString s2(u8"Hello"); // const char8_t* 构造
		UnicodeString s3(nullptr); // 空指针构造(安全 应输出空字符)
		s2.StrPutOut(ConPutStrAct::NeedEndl); // 输出 Hello
		s3.StrPutOut(ConPutStrAct::NotEndl); // 输出空字符 不换行
		std::cout << "<- (这是空字符的结尾)\n";

		// 测试 2: 拷贝构造与赋值运算符
		std::cout << "\n--- 测试 2: 拷贝与赋值 ---\n";
		UnicodeString s4(s2); // 拷贝构造
		s4.StrPutOut(ConPutStrAct::NotEndl); std::cout << " <- 拷贝自 s2\n";

		UnicodeString s5(u8"World");
		s5 = s2; // UnicodeString 赋值
		s5.StrPutOut(ConPutStrAct::NotEndl); std::cout << " <- 赋值自 s2\n";

		s5 = u8"直接赋值的C字符串"; // const char8_t* 赋值
		s5.StrPutOut(ConPutStrAct::NeedEndl);

		// 测试 3: 索引插入(边界与中间)
		std::cout << "\n--- 测试 3: 索引插入测试 (字符个数) ---\n";
		UnicodeString s8(u8"ABCDE");
		std::cout << "原字符串: "; s8.StrPutOut(ConPutStrAct::NeedEndl);

		std::cout << "在字符索引 0 插入 '>>' : ";
		s8.StrIns(u8">>", 0); // 预期: >>ABCDE
		s8.StrPutOut(ConPutStrAct::NeedEndl);

		std::cout << "在字符索引 4 插入 '[]' : ";
		s8.StrIns(u8"[]", 4); // 预期: >>AB[]CDE
		s8.StrPutOut(ConPutStrAct::NeedEndl);

		std::cout << "在字符索引 9 插入 '<<' : ";
		s8.StrIns(u8"<<", 9); // 预期: >>AB[]CDE<<
		s8.StrPutOut(ConPutStrAct::NeedEndl);

		// 测试 4: 多字节字符边界与越界测试 (中英文混合)
		std::cout << "\n--- 测试 4: 多字节字符边界测试 (中英混合) ---\n";
		UnicodeString s9(u8"A你B好C");
		std::cout << "原字符串: "; s9.StrPutOut(ConPutStrAct::NeedEndl);

		// 首部插入
		std::cout << "在字符索引 0 (首部) 插入 '[首]' : ";
		s9.StrIns(u8"[首]", 0); // 预期: [首]A你B好C
		s9.StrPutOut(ConPutStrAct::NeedEndl);

		// 中间插入 (当前长度为 8, 索引 4 是 'B' 之前)
		std::cout << "在字符索引 4 (中间) 插入 '[中]' : ";
		s9.StrIns(u8"[中]", 4); // 预期: [首]A你[中]B好C
		s9.StrPutOut(ConPutStrAct::NeedEndl);

		// 末尾插入 (当前长度为 10, 索引 10 是最后)
		std::cout << "在字符索引 10 (末尾) 插入 '[尾]' : ";
		s9.StrIns(u8"[尾]", 10); // 预期: [首]A你[中]B好C[尾]
		s9.StrPutOut(ConPutStrAct::NeedEndl);

		// 越界巨大索引 (当前长度为 13)
		std::cout << "在字符索引 999 (越界) 插入 '[越界]' : ";
		s9.StrIns(u8"[越界]", 999); // 预期追加到末尾: [首]A你[中]B好C[尾][越界]
		s9.StrPutOut(ConPutStrAct::NeedEndl);

		// 测试 5: 运算符重载 + (拼接)
		std::cout << "\n--- 测试 5: 字符串拼接 ---\n";
		UnicodeString a(u8"你好，");
		UnicodeString b(u8"世界！");
		std::cout << "拼接前 a: "; a.StrPutOut(ConPutStrAct::NeedEndl);
		std::cout << "拼接前 b: "; b.StrPutOut(ConPutStrAct::NeedEndl);

		a + b;
		std::cout << "a + b 后的 a: "; a.StrPutOut(ConPutStrAct::NeedEndl);

		// 测试 6: 对象所有权转移 (Move)
		std::cout << "\n--- 测试 6: 对象所有权转移 ---\n";
		UnicodeString src(u8"我要被移动了");
		UnicodeString dst(u8"目标对象原内容");

		std::cout << "转移前 dst: "; dst.StrPutOut(ConPutStrAct::NeedEndl);
		dst.StrObjOwnToMove(src); // 转移所有权
		std::cout << "转移后 dst: "; dst.StrPutOut(ConPutStrAct::NeedEndl);
		std::cout << "转移后 src (应为空): "; src.StrPutOut(ConPutStrAct::NeedEndl);
		// 注: C++26 版本基于 std::u8string, 移动后源对象是被 clear() 的合法空字符串, 不会像 C++14 版本那样抛出异常.

		// 测试 7: 链式调用
		std::cout << "\n--- 测试 7: 链式调用 ---\n";
		UnicodeString chain(u8"链式");
		chain.StrPutOut_Obj(ConPutStrAct::NotEndl)
			.StrPutOutOther_Obj(u8" 调用", ConPutStrAct::NotEndl)
			.StrPutOut_Obj(ConPutStrAct::NeedEndl);

		// 测试 8: 空指针安全
		std::cout << "\n--- 测试 8: 空指针输出保护 ---\n";
		UnicodeString safe(u8"测试输出:");
		safe.StrPutOut_Obj(ConPutStrAct::NotEndl).StrPutOutOther_Obj(nullptr, ConPutStrAct::NeedEndl); // 不应崩溃

		std::cout << "\n========== 测试结束 ==========\n";
	}
#pragma endregion

/*|||||||||||||||||||||||||||||||||||分割线||||||||||||||||||||||||||||||||||||||||*/
} // namespace UnicodeStringGroup 结束