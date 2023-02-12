// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE 类
#include <afxodlgs.h>       // MFC OLE 对话框类
#include <afxdisp.h>        // MFC 自动化类
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC 数据库类
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO 数据库类
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

//C++标准库头文件
#include <io.h>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <memory>

//公共类
#pragma comment(lib,"BaseForProTookit.lib")
#include "DynaArray.h"
#include "threadpool.h"
#include "Logger.h"

//数据库服务
#pragma comment(lib,"DataBaseService.lib")
#include "DataType.h"
#include "DBService.h"
#include "DBCommonAPI.h"
#include "DBAccessConn.h"

//生产计划计算方法库
#pragma comment(lib,"ProductionPlanCalculateLib.lib")

////功能函数接口类
//全局功能函数头文件
#include "CreoFuntion/CreoCommonFunction.h"
#include "CreoFuntion/CDpview.h"//Creo View控件预览文件

using namespace DataBase;

std::shared_ptr<DBService>& getProPlanDBService();

struct StatusData 
{
	CString strInfo;//状态栏信息
	int32_t nTimeCount;//显示计时
};
#ifdef PRO_USE_VAR_ARGS
////ProTookit头文件
//*************添加Pro/Tookit头文件
#include <ProToolkit.h>
#include <ProUtil.h>
#include <ProArray.h>
#include <ProDimension.h>
#include <ProWindows.h>
#include <ProSolid.h>
#include <ProParameter.h>
#include <ProParamval.h>
#include <ProMessage.h>
#include <ProMenu.h>
#include <ProMenuBar.h>
#include <ProMdl.h>
#include <ProModelitem.h>
#include <ProDrawing.h>
#include <ProFeature.h>
#include <ProFeatType.h>
#include <ProDrawing.h>
#include <ProNotify.h>
#include <ProRefInfo.h>
#include <ProRelSet.h>
#include <ProSelection.h>
#include <ProUICmd.h>
#include <ProNotify.h>
#include <ProPopupmenu.h>
#include <ProSelbuffer.h>
#include <ProAsmcomp.h>
#include <ProAsmcomppath.h>
#include <afxdisp.h>
#include <afxcontrolbars.h>
//////清除工作目录重复文件专有
#include <ProUICmd.h>
#include <ProUIMessage.h>
#include <ProToolkitDll.h>
////一键着色专有
#include <ProSurface.h>
#include <afxdisp.h>

#endif
//全局函数

