// CDpview.h : 由 Microsoft Visual C++ 创建的 ActiveX 控件包装类的声明

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CDpview

class CDpview : public CWnd
{
protected:
	DECLARE_DYNCREATE(CDpview)
// 特性
public:
	void ShowModelToCreoView(CString mdlpath);//预览Creo View 预览用
};
