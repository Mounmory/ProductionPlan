// stdafx.cpp : 只包括标准包含文件的源文件
// ADOConn.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用

#ifdef PRO_USE_VAR_ARGS	//在Creo中注册使用dll
#include <ProMessage.h>

//注册Creo的dll接口
extern "C" int user_initialize(int argc, char *argv[])
{
	ProMessageDisplay(L"message.txt", "Starting CreTookitDevelop Success!");//头文件 ProMessage.h.此处随便添加一个ProE函数即可
	return PRO_TK_NO_ERROR;
}
extern "C" void user_terminate()
{

}

#endif
