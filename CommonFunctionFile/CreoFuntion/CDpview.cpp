// CDpview.cpp : 由 Microsoft Visual C++ 创建的 ActiveX 控件包装类的定义

#include "stdafx.h"
#include "CDpview.h"

/////////////////////////////////////////////////////////////////////////////
// CDpview

IMPLEMENT_DYNCREATE(CDpview, CWnd)

// CDpview 属性

// CDpview 操作
void CDpview::ShowModelToCreoView(CString mdlpath)//预览PTC Creo View 预览用
{
	SetProperty(0x1, VT_BSTR, mdlpath);
}
