#ifndef MYSTL_ASTRING_H_
#define MYSTL_ASTRING_H_

// 定义了 string, wstring, u16string, u32string 类型

#include "basic_string.h"	//string的真正实现

namespace mystl {

	using string = mystl::basic_string<char>;
	using wstring = mystl::basic_string<wchar_t>;
	using u16string = mystl::basic_string<char16_t>;
	using u32string = mystl::basic_string<char32_t>;

}	//命名空间结束符



#endif // !MYSTL_ASTRING_H_

